#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_TOKENS 100
#define DELIMIT " \t\r\n\a" 
#define MAX_INPUT_SIZE 1000
#define EXIT_USSH 5 
#define NO_INPUT NULL

void die(char *msg);
char *ussh_read(void);
void ussh_catch_signal(void);
char **ussh_parse(char* text);
int ussh_execute(char **args);

int main(void) 
{
  char **args;
  char *input;

  ussh_catch_signal();

  while (1) {
    input = ussh_read();
    if (input == NO_INPUT) {
      free(input);
      continue;
    }

    args = ussh_parse(input);
    if (ussh_execute(args) == EXIT_USSH) {
      free(input);
      exit(0);
    }
    free(input);

    wait(NULL);
  }

  return 0;
}

char **ussh_parse(char *text)
{
  size_t i;
  size_t tokens;
  char **arr = malloc(sizeof(char *) * MAX_TOKENS);
  if (arr == NULL) {
    die("malloc");
  }
  char *token;

  token = strtok(text, DELIMIT);
  tokens = MAX_TOKENS;
  i = 0;
  while (token != NULL) {
    arr[i] = token;
    i++;

    if (i >= tokens) {
      tokens *= 2;
      arr = realloc(arr, tokens * sizeof(char * )); 
    }
    
    token = strtok(NULL, DELIMIT);
  }
  arr[i] = NULL;

  return arr;
}

char *ussh_read(void)
{
  char c;
  char *input;
  size_t sz;
  size_t i = 0;
  input = calloc(MAX_INPUT_SIZE, sizeof(char));

  if (input == NULL) {
    die("malloc");
  }

  fflush(stdout);
  printf("? ");

  sz = MAX_INPUT_SIZE;
  while ((c = getchar()) != EOF && c != '\n') {
    if (i >= sz) {
      sz *= 2;
      input = realloc(input, sizeof(char) * sz);
    }

    input[i] = c;
    i++;
  }

  if (input[0] != 0) return input;
  else return NO_INPUT; 
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
        perror("cd");
      }
  } else if (strncmp(args[0], "help", MAX_INPUT_SIZE) == 0) {
      printf("ussh v0.01\n Enter a command.\n");
  } else {
    child = fork();
    if (child == -1) {
      die("fork");
    } else if (child == 0) {
      if (execvp(args[0], args) == -1) {
        die(args[0]);
      }
    }

    free(args);
  }

  return 0;
}

void die(char *msg) {
  perror(msg);
  exit(-1);
}

void ussh_catch_signal(void) {
  signal(SIGINT, SIG_IGN);
  signal(SIGQUIT, SIG_IGN);
  signal(SIGTERM, SIG_IGN);
  signal(SIGSEGV, SIG_IGN);
  signal(SIGSYS, SIG_IGN);
  signal(SIGHUP, SIG_IGN);
  signal(SIGABRT, SIG_IGN);
}
