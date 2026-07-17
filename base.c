#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_INPUT_SIZE 100

char *ussh_read();

int main(void) 
{

  while (1) {
    pid_t child;
    char *input;

    input = ussh_read();
    if (strncmp(input, "exit", MAX_INPUT_SIZE) == 0) {
      exit(0);
    }

    child = fork();
    
    if (child == -1) {
      perror("fork");
      exit(EXIT_FAILURE);
    }

    if (child == 0) {
      execlp(input, input, NULL);
      perror(input);
      exit(EXIT_FAILURE);
    }

    free(input);

    wait(NULL);
  }
  return 0;
}

char *ussh_read()
{
  char *input;
  input = malloc(MAX_INPUT_SIZE * sizeof(char));
  
  printf("? ");
  fflush(stdout);
  if (fgets(input, sizeof(input), stdin) == NULL)
    exit(-1);

  input[strcspn(input, "\n")] = '\0';
  return input;
}
