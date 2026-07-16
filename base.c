#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_INPUT_SIZE 100

int main(void) 
{
    char input[MAX_INPUT_SIZE];

    while (1) {
        printf("? ");
        fflush(stdout);
        if (fgets(input, sizeof(input), stdin) == NULL)
            break;

        input[strcspn(input, "\n")] = '\0';

        if (strncmp(input, "exit", MAX_INPUT_SIZE) == 0) {
            exit(0);
        }
        
        pid_t child = fork();
        
        if (child == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (child == 0) {
            execlp(input, input, NULL);
            perror(input);
            exit(EXIT_FAILURE);
        }

        wait(NULL);
    }
    return 0;
}
