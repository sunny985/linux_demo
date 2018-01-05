#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> /* mkfifo */
#include <sys/stat.h>

#define FIFO_NAME "fifo_test"

int main(void)
{
    int fifo;

    fifo = mkfifo(FIFO_NAME, 0666);
	if (fifo == -1) {
		perror("fifo");
		exit(EXIT_FAILURE);
	}

	return 0;
}