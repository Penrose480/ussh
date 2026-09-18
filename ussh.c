#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

/*** Definitions ***/

#define BASE_TOKENS 10
#define DELIMIT " \t\r\n\a" 
#define BASE_INPUT_SIZE 100
#define EXIT_USSH 5 
#define NO_INPUT NULL

/*** Prototypes ***/

void die(char *msg);
void do_nothing(int sig);
char *ussh_read(void);
void ussh_catch_signal(void);
char **ussh_parse(char* text);
int ussh_execute(char **args);

int main(void) 
{
  char **args;
  char *input;

  ussh_catch_signal();

  /* Main loop */
  for (;;) {
    input = ussh_read();
    if (input == NO_INPUT) {
      free(input);
      continue;
    }

    args = ussh_parse(input);
    if (ussh_execute(args) == EXIT_USSH) {
      free(input);
      break;
    }
    free(input);

    wait(NULL);
  }

  return 0;
}

/** Input **/
char *ussh_read(void)
{
  char c;
  char *input;
  size_t sz;
  size_t i = 0;
  input = calloc(BASE_INPUT_SIZE, sizeof(char));

  if (input == NULL) {
    die("malloc");
  }

  fflush(stdout);
  printf("? ");

  sz = BASE_INPUT_SIZE;

  /* Increase buffer size if fully used */
  while ((c = getchar()) != EOF && c != '\n') {
    if (i >= sz) {
      sz += BASE_INPUT_SIZE;
      input = realloc(input, sizeof(char) * sz);
      if (input == NULL) {
        die("malloc");
      }
    }

    input[i] = c;
    i++;
  }

  if (input[0] != 0) return input;
  else return NO_INPUT; 
}

/** Parse **/
char **ussh_parse(char *text)
{
  size_t i;
  size_t tokens;
  char *token;
  char **arr = malloc(sizeof(char *) * BASE_TOKENS);
  if (arr == NULL) {
    die("malloc");
  }

  /* Parse through text */
  token = strtok(text, DELIMIT);
  tokens = BASE_TOKENS;
  i = 0;
  while (token != NULL) {
    arr[i] = token;
    i++;

    if (i >= tokens) {
      tokens += BASE_TOKENS;
      arr = realloc(arr, tokens * sizeof(char * )); 
      if (arr == NULL) {
        die("malloc");
      }
    }
    
    token = strtok(NULL, DELIMIT);
  }
  arr[i] = NULL;

  return arr;
}


/** Execute **/
int ussh_execute(char **args) {
  pid_t child;

  /* Execute diff builtin based on input */
  if (strncmp(args[0], "exit", BASE_INPUT_SIZE) == 0) {  
    if (args[1] != NULL) {
      printf("Usage: exit\n");
    } else {
      free(args);
      return EXIT_USSH;
    }
  } else if (strncmp(args[0], "cd", BASE_INPUT_SIZE) == 0) {
      if (args[1] == NULL) {
        chdir("/home");
      } else if (chdir(args[1]) == -1) {
        perror("cd");
      }
  } else if (strncmp(args[0], "help", BASE_INPUT_SIZE) == 0) {
    if (args[1] != NULL) {
      printf("Usage: help\n");
    } else {       
      printf ("ussh v0.01\n Enter a command.\n");
    }
  /* If not builtin - execute normally */
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

/** Error handling **/
void die(char *msg) {
  perror(msg);
  exit(-1);
}

/** Dummy function **/
void do_nothing(int sig) {
  sig = sig;
}

/** Signal handling - ignore all signals **/
void ussh_catch_signal(void) {
  signal(SIGINT, do_nothing);
  signal(SIGQUIT, do_nothing);
  signal(SIGTERM, do_nothing);
  signal(SIGSEGV, do_nothing);
  signal(SIGSYS, do_nothing);
  signal(SIGHUP, do_nothing);
  signal(SIGABRT, do_nothing);
}
