#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) 
{
    char input[100];

    while (1) {
	printf("? ");
	fflush(stdout);
	if (fgets(input, sizeof(input), stdin) == NULL)
	    break;

	input[strcspn(input, "\n")] = '\0';
	
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
