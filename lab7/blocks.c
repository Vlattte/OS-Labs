#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

int arr = 0;
static pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;

void* write_fun(void* str)
{
	while (1)
	{
		pthread_rwlock_wrlock(&rwlock);
		arr++;
		pthread_rwlock_unlock(&rwlock);
		sleep(2);
	}
}

void* read_fun(void* str)
{
	while (1)
	{
		pthread_rwlock_rdlock(&rwlock);
		printf("Cur val = %d\nPthread ID - %x\n", arr, pthread_self());
		pthread_rwlock_unlock(&rwlock);
		sleep(2);
	}
	pthread_exit(NULL);
}

int main(int argc, char** argv)
{
	pthread_t writer;
	pthread_create(&writer, NULL, write_fun, NULL);
	
	pthread_t reader[10];
	for (int i = 0; i < 10; i++)
		pthread_create(&reader[i], NULL, read_fun, NULL);
	

	pthread_join(writer, NULL);
	pthread_rwlock_destroy(&rwlock);
	return 0;
}


