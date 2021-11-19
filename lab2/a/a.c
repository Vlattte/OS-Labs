#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

typedef struct data_struct
{
    time_t parent_time;
    pid_t cur_pid;
} data_struct;


int main(int argc, char** argv)
{
    int pipedec[2];
    pid_t pid;
    pipe(pipedec);

    switch (pid = fork())
    {
        case -1:
            printf("Error\n");
            return -1;
        case 0:
            printf("{CHILD}");
            
            sleep(5);
            data_struct datus;
            close(pipedec[1]);
            read(pipedec[0], &datus, sizeof(data_struct));
            close(pipedec[0]);

            time_t child_time = time(NULL);
            time_t temp = datus.parent_time;
            printf("\nCHILD time: %sPARENT pid: %d\n", ctime(&child_time), datus.cur_pid);
            printf("PARENT time: %s", ctime(&datus.parent_time));
            return 0;
        default:
            printf("{PARENT}");
            time_t parent_time = time(NULL);
            data_struct data = { parent_time, getpid() };
            close(pipedec[0]);
            write(pipedec[1], (void*)(&data), sizeof(data_struct));
            close(pipedec[1]);
            printf("\nCURRENT pid: %d\n", data.cur_pid);
            wait(0);
            return 0;
    }
    return 0;
}
