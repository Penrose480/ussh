#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_TOKENS 60
#define DELIMIT " \t\r\n\a" 
#define MAX_INPUT_SIZE 100
#define EXIT_USSH 5 

char *ussh_read(void);
char **ussh_parse(char* text);
void do_nothing(int sig);
int ussh_execute(char **args);

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
    if (ussh_execute(args) == EXIT_USSH)
      exit(0);
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

int ussh_execute(char **args) {
  pid_t child;

    if (strncmp(args[0], "exit", MAX_INPUT_SIZE) == 0) {  
      free(args);
      return EXIT_USSH;
    } else if (strncmp(args[0], "cd", MAX_INPUT_SIZE) == 0) {
      if (args[1] == NULL) {
        chdir("/home");
      } else if (chdir(args[1]) == -1) {
        fprintf(stderr, "Invalid directory\n");
      }
    } else if (strncmp(args[0], "help", MAX_INPUT_SIZE) == 0) {
      printf("ussh v0.01\n Enter a command.\n");
    } else {
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
    free(args);
  }

  return 0;
}

void do_nothing(int sig) {
  sig = sig;
}
