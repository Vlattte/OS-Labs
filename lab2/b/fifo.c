#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>

#define FIFO_NAME "temp_fifo"

typedef struct sended_data
{
	time_t writer_time;
	pid_t writer_pid;
} sended_data;

int main(int argc, char** argv)
{
	pid_t pid;
	int status;

	switch (pid = fork())
	{
		case -1:
			printf("ERROR IN FORK: %s\n", strerror(errno));
			break;
		case 0:
			sleep(1);
			int fifo_read = open(FIFO_NAME, O_RDONLY);
			if (fifo_read < 0)
			{
				printf("{Reading process} FIFO read error: %s\n", strerror(errno));
				return -1;
			}

			sleep(5);
			sended_data reader_data;
			read(fifo_read, &reader_data, sizeof(sended_data));
			close(fifo_read);

			time_t reader_time = time(NULL);
			printf("{Reading process}\n===>writing process time: %s===>writing process pid: %d\n",
				ctime(&reader_data.writer_time), reader_data.writer_pid);
			printf("===>reading process time: %s", ctime(&reader_time));
			return 0;
		default:
			unlink(FIFO_NAME);
			int fifo = mkfifo(FIFO_NAME, 0777);
			if (fifo < 0)
			{
					printf("{Writing process} FIFO creation error: %s\n", strerror(errno));
					return -1;
			}
			int fifo_writer = open(FIFO_NAME, O_WRONLY);

			if (fifo_writer < 0)
			{
				printf("FIFO write error : %s\n", strerror(errno));
				return -1;
			}

			time_t writer_time = time(NULL);
			pid_t cur_pid = getpid();
			printf("{Writing process}\n===>writing process pid: %d\n", getpid());

			sended_data writer_data = { writer_time, cur_pid };
			write(fifo_writer, &writer_data, sizeof(sended_data));
			close(fifo_writer);
			wait(&status);
			return 0;
	}
	return 0;
}

