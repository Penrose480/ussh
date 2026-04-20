#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) 
{
//    while (1) {
	char input;
	scanf("? %s", &input);
	
	pid_t child = fork();
	
	if (child == -1) {
	    perror("fork");
	    exit(EXIT_FAILURE);
	}

	input[strchr(&input, '\n')] = '\0';

	if (child == 0) {
	    execlp(&input, &input, NULL);
	    perror(&input);
	    exit(1);
	}
	wait(NULL);

//    }
}
