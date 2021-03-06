#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

int arr = 0;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void* write_fun(void* str)
{
	while (1)
	{
		pthread_mutex_lock(&mutex);
		arr++;
		pthread_mutex_unlock(&mutex);
		sleep(2);
	}
}

void* read_fun(void* str)
{
	while (1)
	{
		pthread_mutex_lock(&mutex);
		printf("Cur val = %d\n", arr);
		printf("Pthread ID - %x\n", pthread_self());
		pthread_mutex_unlock(&mutex);
		sleep(2);
	}
	pthread_exit(NULL);
}

int main(int argc, char** argv)
{
	pthread_mutex_init(&mutex, NULL);

	pthread_t writer;
	pthread_create(&writer, NULL, write_fun, NULL);
	
	pthread_t reader[10];
	for (int i = 0; i < 10; i++)
		pthread_create(&reader[i], NULL, read_fun, NULL);

	pthread_join(writer, NULL);
	pthread_mutex_destroy(&mutex);
	return 0;
}


