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

int main(int argc, char** argv)
{
	key_t key = ftok(FILENAME, 1);
	if (key == -1)
	{
		printf("FTOK ERROR %s\n", strerror(errno));
		exit(-1);
	}

	int shmemid = shmget(key, 10 * sizeof(int), IPC_CREAT | 0666);
	if (shmemid < 0)
	{
		printf("SHMGET ERROR %s\n", strerror(errno));
		exit(-2);
	}

	struct shmid_ds shmid_ds;
	shmctl(shmemid, IPC_STAT, &shmid_ds);

	if (shmid_ds.shm_nattch >= 1)
	{
		printf("ERROR!!!! PROCCES ALREADY EXISTS!!!!\n");
		exit(1);
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

	time_t cur_time;

	semop(semid, &sem_open, 1);
	while (1 == 1)
	{
		semop(semid, &sem_lock, 1);
		cur_time = time(NULL);
		data shared_memory = { getpid(),  cur_time };
		*((data*)shmatptr) = shared_memory;
		
		sleep(5);

		semop(semid, &sem_open, 1);
		printf("\nplaced in shared memory: MY PID: %d\nMY TIME: %s\n",
			shared_memory.fppid, ctime(&shared_memory.fptime));

		shmdt(shmatptr);

		shmatptr = (data*)shmat(shmemid, NULL, 0);
	}


	semctl(semid, 0, IPC_RMID);

	semctl(shmemid, IPC_RMID, 0);

	return 0;
}
