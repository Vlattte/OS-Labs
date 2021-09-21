#include <unistd.h>
#include <stdio.h>
#include <pwd.h>
#include <grp.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <time.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stdint.h>
#include <string.h>

int main(int argc, char** argv)
{
	int go; //getopt variable
	
	//current dir and the num of the path in console
	struct dirent* files;
	DIR* dir;
	int l_flag = 0;
	int path_num = -1;
	int cur_par = 1;

	while(argc > cur_par)
	{
		//if flag, then operate it
		if ((go = getopt(argc, argv, "l")) != -1)
	       	{
	   	        switch(go)
	                {
	               	        case 'l':
				l_flag = 1;
	                        break;
	                     	case '?':
        	       	        printf("ERROR");
				exit(2);
	              	}
        	}
		else 
		{
			path_num = cur_par;
		}
		cur_par = cur_par + 1;
	}		

	struct dirent* cdir;
	if(path_num != -1)
	{	
		dir = opendir(argv[path_num]);
		if(dir == NULL)
		{
			printf("NO SUCH DIRECTORY");
			exit(3);
		}
	}
	else
	{
		dir = opendir(".");	
	}
	
	struct stat st;
	if(path_num != -1 && l_flag == 1)
	{
                stat(argv[path_num], &st);
		
		printf("\ntotal %ld\n", st.st_blocks);
	}	
	else if(l_flag == 1 && path_num == -1)
	{
		stat(".", &st);
		printf("\ntotal %d\n", st.st_blocks);
	}

	while(cdir = readdir(dir))        	
	{
        	        struct stat buff;
			if(path_num != -1)
			{
				char* path = malloc(strlen(argv[path_num]) + strlen(cdir->d_name) + 1);
				memcpy(path, argv[path_num], strlen(argv[path_num]));
				memcpy(path + strlen(argv[path_num]), cdir->d_name, strlen(cdir->d_name) + 1);
                        	stat(path, &buff);
				free(path);
                        }
			else
			{
				stat(cdir->d_name, &buff);				
			}
				
			if(cdir->d_name[0] != '.')
			{
				if(l_flag == 1)
                		{

				switch(buff.st_mode & S_IFMT)
				{
					case S_IFSOCK: printf("s"); break;
					case S_IFLNK: printf("l"); break; 
                                        case S_IFBLK: printf("b"); break;
                                        case S_IFDIR: printf("d"); break;
                                        case S_IFCHR: printf("c"); break;
                                        case S_IFIFO: printf("p"); break;
                                        default: printf("-"); break;         
				}

				//user block
				if(S_IRUSR & buff.st_mode) printf("r"); else printf("-");
				if(S_IWUSR & buff.st_mode) printf("w"); else printf("-");
				if(S_IXUSR & buff.st_mode) printf("x"); else printf("-");
				//group block
				if(S_IRGRP & buff.st_mode) printf("r"); else printf("-");
				if(S_IWGRP & buff.st_mode) printf("w"); else printf("-");
				if(S_IXGRP & buff.st_mode) printf("x"); else printf("-");
				//others block
				if(S_IROTH & buff.st_mode) printf("r"); else printf("-");
				if(S_IWOTH & buff.st_mode) printf("w"); else printf("-");
				if(S_IXOTH & buff.st_mode) printf("x"); else printf("-");
				struct passwd* user = getpwuid(buff.st_uid);
				struct group* group = getgrgid(buff.st_gid);
				
				printf(" %d ", buff.st_nlink);
					
				//if no user
				if(user == 0)
					printf("============== ");
				else
					printf("%s ", user->pw_name);
				//if no group 
				if(group == 0)
					printf("============== ");
				else
					printf("%s ", group->gr_name);

				printf("%ld ", buff.st_size);
				//data
				time_t t = buff.st_ctime;
                                struct tm ftime;
                                time_t now;                                
				localtime_r(&t, &ftime);
				time(&now);
				
				switch(ftime.tm_mon)
				{
					case 0: printf("Jan"); break;
					case 1: printf("Feb"); break;
					case 2: printf("Mar"); break;
					case 3: printf("Apr"); break;
					case 4: printf("May"); break;
					case 5: printf("Jun"); break;
					case 6: printf("Jul"); break;
					case 7: printf("Aug"); break;
					case 8: printf("Sep"); break;
					case 9: printf("Oct"); break;
					case 10:printf("Nov"); break;
					case 11:printf("Dec"); break;
				}
				
				if(localtime(&now)->tm_year == localtime(&buff.st_ctime)->tm_year)
                                {
					printf(" %d %02d:%02d ", ftime.tm_mday, ftime.tm_hour, ftime.tm_min);
		                }
				else
				{	
					printf(" %d %d ", ftime.tm_mday, 1900 + ftime.tm_year);
				}
				}
				printf("%s ", cdir->d_name);//file name
				if(l_flag == 1)
					printf("\n");
			}
					
	}	
	printf("\n");	
	closedir(dir);
	return 0;
}
