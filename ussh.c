#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_TOKENS 60
#define DELIMIT " "
#define MAX_INPUT_SIZE 100

char **ussh_read(void);
char **ussh_parse(char* text);

int main(void) 
{
  while (1) {
    pid_t child;
    char **args;

    args = ussh_read();

    if (strncmp(args[0], "exit", MAX_INPUT_SIZE) == 0)  
      exit(0);
  
    child = fork();
    if (child == -1) {
      perror("fork");
      exit(-1);
    }
    if (child == 0) {
      if (execvp(args[0], args) == -1) {
        perror(args[0]);
        exit(-1);
      }
    }

    free(args);

    wait(NULL);
  }
  return 0;
}

char **ussh_parse(char *text)
{
  size_t i;
  char **arr = malloc(sizeof(char *) * MAX_TOKENS);
  if (arr == NULL) {
    fprintf(stderr, "Malloc err");
    exit(-1);
  }
  char *token;

  text[strcspn(text, "\n")] = '\0';

  token = strtok(text, DELIMIT);
  i = 0;
  while (token != NULL) {
    arr[i] = token;
    i++;

    if (i > MAX_TOKENS) {
      fprintf(stderr, "Too large input\n");
      exit(-1);
    }
    
    token = strtok(NULL, DELIMIT);
  }
  arr[i] = NULL;

  return arr;
}

char **ussh_read(void)
{
  char *input;
  input = malloc(MAX_INPUT_SIZE * sizeof(char));

  if (input == NULL) {
    fprintf(stderr, "Malloc err");
    exit(-1);
  }

  fflush(stdout);
  printf("? ");

  if (fgets(input, sizeof(input), stdin) == NULL) {
    perror("fgets");
    exit(-1);
  } 

  return ussh_parse(input);
}
