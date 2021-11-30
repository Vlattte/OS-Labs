#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define FILENAME "sem_memory_buffer"

struct sembuf sem_lock = { 0, -1, 1 }, sem_open = { 0, 1, 0 };

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

	int shmemid = shmget(key, sizeof(data), 0666);
	if (shmemid < 0)
	{
		printf("SHMGET ERROR %s\n", strerror(errno));
		exit(-2);
	}

	int semid = semget(key, 1, IPC_CREAT | 0666);
	if (semid < 0)
	{
		printf("SEMGET ERROR %s\n", strerror(errno));
		exit(-2);
	}

	data* shmatptr = (data*)shmat(shmemid, NULL, 0);
	if (shmatptr < 0)
	{
		printf("SHMAT ERROR %s\n", strerror(errno));
		exit(-3);
	}

	

	semop(semid, &sem_lock, 1);

	data getted_memory = *((data*)shmatptr);
	printf("\ngetted from shared memory: GIVER PID: %d\nGIVER TIME: %s",
		getted_memory.fppid, ctime(&getted_memory.fptime));
	time_t cur_time = time(NULL);
	printf("GETTER TIME: %s\n", ctime(&cur_time));

	semop(semid, &sem_open, 1);

	shmdt(shmatptr);

	return 0;
}
