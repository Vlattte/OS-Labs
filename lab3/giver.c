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
	struct shmid_ds shmid_ds;

	key_t key = ftok(FILENAME, 1);
	if (key == -1)
	{
		printf("FTOK ERROR %s\n", strerror(errno));
		exit(-1);
	}

	int shmemid = shmget(key, 1, IPC_CREAT | 0666);
	if (shmemid < 0)
	{
		printf("SHMGET ERROR %s\n", strerror(errno));
		exit(-2);
	}

	shmctl(shmemid, IPC_STAT, &shmid_ds);
	
	if (shmid_ds.shm_nattch >= 1)
	{
		printf("ERROR!!!! PROCCES ALREADY EXISTS!!!!\n");
		exit(1);
	}

	void* shmatptr = shmat(shmemid, NULL, 0);
	if (shmatptr < 0)
                {
                        printf("SHMAT ERROR %s\n", strerror(errno));
                        exit(-3);
                }


	while (1 == 1)
	{
		time_t cur_time = time(NULL);
		data shared_memory = { getpid(),  cur_time };

		*((data*)shmatptr) = shared_memory;
		printf("\nplaced in shared memory: MY PID: %d\nMY TIME: %s\n",
			shared_memory.fppid, ctime(&shared_memory.fptime));
		sleep(5);
	}
	shmdt(shmatptr);
	shmctl(shmemid, IPC_RMID, NULL);
	return 0;
}
