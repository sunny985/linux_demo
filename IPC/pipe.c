#include <stdio.h>
#include <unistd.h> /* pipe */
#include <stdlib.h> /* exit */
#include <string.h> /* strlen */

/* pipe:无名管道，一般用于父子进程通讯
 * 参考 : http://www.cnblogs.com/biyeymyhjob/archive/2012/11/03/2751593.html
 * exit()和_exit()区别，是否刷新文件缓冲区，
 * 参考 : http://blog.sina.com.cn/s/blog_605f5b4f0100x3v0.html
 */
int main(int argc, char *argv[])
{
    int pipefd[2]; /* pipefd[0]:read pipefd[1]:write */
	pid_t pid;
	char c;

	if(argc < 2) {
		fprintf(stderr, "Usage: %s <string>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	if(pipe(pipefd) == -1) {
		perror("pipe");
		exit(EXIT_FAILURE);
	}

	if((pid = fork()) == -1) {
		perror("fork");
		exit(EXIT_FAILURE);
	}

	if (pid == 0) { /* child process read from pipe */
		close(pipefd[1]); /* close unused write end */
		while(read(pipefd[0], &c, 1) > 0) {
			putchar(c);
			putchar('\n');
			close(pipefd[0]);
			_exit(EXIT_SUCCESS);
		}
	}
	else { /* parent process write to pipe*/
		close(pipefd[0]); /* close unused read end */
		write(pipefd[1], argv[1], strlen(argv[1]));
		close(pipefd[1]);
		wait(NULL);
		exit(EXIT_SUCCESS);
	}

    return 0;
}
