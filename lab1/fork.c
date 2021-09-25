#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void at_exit()
{
	printf("HI, i'm Atexit called function, i'm done\n");
}

int main()
{
	pid_t pid;
  	int status;
	
	atexit(at_exit);
  	switch(pid=fork()) 
  	{
		case -1:
			exit(1);
		case 0:
			printf("------\nCHILD:\n------\n");
			printf("Я потомок процесса: %d\n", getppid());
			printf("Мой PID: %d\n", getpid());
			exit(status);
		default:
			printf("-------\nPARENT:\n-------\n");
			printf("Мой PID: %d\n", getpid());
			printf("Мой родительский процесс: %d\n", getppid());
			printf("Ожидание завершения потомка.\n");
			wait(&status);
	}
}