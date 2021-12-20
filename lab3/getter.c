#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/shm.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define FILENAME "memory_buffer"

typedef struct data
{
	pid_t fppid;	//first programm pid
	time_t fptime;	//first programm time
} data;

int main(int agrc, char* argv[])
{
	key_t key = ftok(FILENAME, 1);
	
	if (key == -1)
	{
		printf("FTOK ERROR %s\n", strerror(errno));
		exit(-1);
	}

	int shmemid = shmget(key, 1, 0666);
	if (shmemid < 0)
	{
		printf("SHMGET ERROR %s\n", strerror(errno));
		exit(-2);
	}

	void* shmatptr = shmat(shmemid, NULL, 0);
	if (shmatptr < 0)
	{
		printf("SHMAT ERROR %s\n", strerror(errno));
		exit(-3);
	}

	data getted_memory = *((data*)shmatptr);
	printf("\ngetted from shared memory: GIVER PID: %d\nGIVER TIME: %s", 
		getted_memory.fppid, ctime(&getted_memory.fptime));
	time_t cur_time = time(NULL);
	printf("GETTER TIME: %s\n", ctime(&cur_time));

	shmdt(shmatptr);
//	shmctl(shmemid, IPC_RMID, NULL);

	return 0;
}
