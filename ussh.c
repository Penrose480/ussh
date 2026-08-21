#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_TOKENS 60
#define DELIMIT " \t\r\n\a" 
#define MAX_INPUT_SIZE 100

char *ussh_read(void);
char **ussh_parse(char* text);
void do_nothing(int sig);
void ussh_execute(char **args);

int main(void) 
{
  char **args;
  char *input;

  signal(SIGINT, do_nothing);
  signal(SIGQUIT, do_nothing);
  signal(SIGSTOP, do_nothing);
  signal(SIGTERM, do_nothing);
  signal(SIGSEGV, do_nothing);

  while (1) {
    input = ussh_read();
    if (input == NULL) {
      free(input);
      continue;
    }
    args = ussh_parse(input);

    if (strncmp(args[0], "exit", MAX_INPUT_SIZE) == 0) {  
      free(args);
      free(input);
      exit(0);
    } else if (strncmp(args[0], "cd", MAX_INPUT_SIZE) == 0) {
        if (args[1] == NULL) {
          fprintf(stderr, "Expected cd path/to\n");
        } else if (chdir(args[1]) == -1) {
          fprintf(stderr, "Invalid directory\n");
        }
    } else if (strncmp(args[0], "help", MAX_INPUT_SIZE) == 0) {
        printf("ussh v0.01\n Enter a command.\n");
    } else {
        ussh_execute(args);
    }

    free(args);
    free(input);

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

char *ussh_read(void)
{
  char *input;
  input = malloc(MAX_INPUT_SIZE * sizeof(char));

  if (input == NULL) {
    fprintf(stderr, "Malloc err");
    exit(-1);
  }

  fflush(stdout);
  printf("? ");

  if (fgets(input, MAX_INPUT_SIZE, stdin) == NULL) {
    perror("fgets");
    exit(-1);
  } 

  if (*input != '\n') return input;
  else return NULL; 
}

void ussh_execute(char **args) {
  pid_t child;

  child = fork();
  if (child == -1) {
    perror("fork");
    exit(-1);
  } else if (child == 0) {
    if (execvp(args[0], args) == -1) {
      perror(args[0]);
      exit(-1);
    }
  }
}

void do_nothing(int sig) {
  sig = sig;
}
