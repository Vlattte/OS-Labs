#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

int CountAmount(char* arch_name, char* file_name, int file_scan)
{
	FILE* archivator;
	if ((archivator = fopen(arch_name, "r")) == NULL)
	{
		return 0;
	}

	fseek(archivator, 0, SEEK_END);
	int length = ftell(archivator);
	fseek(archivator, 0, SEEK_SET);

	int file_amount = 0;
	mode_t mode_len;
	long int file_size;
	int end_flag = 0;
	char ch = '|';
	int n = 0;
	
	if (file_scan == 0)
		printf("Files in arch:");

	while (end_flag != 1)
	{
		long name_len;
		fscanf(archivator, "%ld ", &name_len);
		char *cur_file_name;
		cur_file_name = malloc(name_len);
		fread(cur_file_name, 1, name_len, archivator);
		fscanf(archivator, " %d %ld", &mode_len, &file_size);
		
	

		if (file_scan == 0)
			printf("\n\t%s", cur_file_name);
		else if (strcmp(cur_file_name, file_name) == 0)
		{
			free(cur_file_name);
			printf("\n------------------------------------------------------\nThere is already file with this name (%s) ERROR\n------------------------------------------------------\n", file_name);
			return -1;
		}


		file_amount += 1;
		n = 0;
		while (n < (file_size + 1))
		{
			n += 1;
			ch = fgetc(archivator);
			if (ftell(archivator) == length)
			{
				end_flag = 1;
			}
		}
		free(cur_file_name);
	}

	fclose(archivator);
	return file_amount;
}

//move file to arch_file
void insertion(char* arch_name, char* file_name)
{
	int ardy_exts = CountAmount(arch_name, file_name, 1);
	if (ardy_exts == -1)
		return;

	FILE* file, * archivator;

	
	if ((file = fopen(file_name, "r")) == NULL)
	{
		printf("Cannot open the file %s\n", file_name);
		exit(2);
	}

	struct stat file_stat;
	stat(file_name, &file_stat);
	mode_t mode = file_stat.st_mode;

	archivator = fopen(arch_name, "a+");
	char* buff = 0;
	long length;
	if (file)
	{
		fseek(file, 0, SEEK_END);
		length = ftell(file);
		fseek(file, 0, SEEK_SET);

		buff = malloc(length);
		if (buff)
		{
			fread(buff, 1, length, file);
		}
	}

	fprintf(archivator, "%d %s %d %d\n", strlen(file_name), file_name, mode, length);
	fwrite( buff,1,length, archivator);

	free(buff);
	fclose(file);
	fclose(archivator);
}


//extract current file_name from current archive
void extraction(char* arch_name, char* file_name)
{
	FILE* file, * archivator;
	if ((archivator = fopen(arch_name, "r")) == NULL)
	{
		printf("Cannot open the file: (%s)\n", arch_name);
		return;
	}


	char ch;
	fpos_t pos;
	long pars_len;
	long name_len;
	int search_flag = 1;

	long int before_file_size = 0;
	long int file_size;
	int mode_len = 0;


	while (search_flag != 0)
	{
		file_size = 0;
		pars_len = 0;
		name_len = 0;

		//name_len
		fgetpos(archivator, &pos);
		ch = ' ';
		while (ch != '\n')
		{
			ch = fgetc(archivator);
			if (ch == EOF)
			{
				/*ch = '\n';
				search_flag = 0;*/
				printf("No such file in archive.\nFile name: %s\nArchive name: %s\n", file_name, arch_name);
				return;
			}
			pars_len += 1;
		}

		fsetpos(archivator, &pos);
		//takes current file name, permessions and file size
		fscanf(archivator, "%ld ", &name_len);
		char *cur_file_name;
		cur_file_name = malloc(name_len);
		fread(cur_file_name, 1, name_len, archivator);
		fscanf(archivator, " %d %ld", &mode_len, &file_size);

		//if our file
		if (strcmp(cur_file_name, file_name) == 0)
		{
			file = fopen(file_name, "w");
			char* _buff;
			if (archivator)
			{
				_buff = malloc(file_size);
				if (_buff)
				{
					ch = fgetc(archivator);
					fread(_buff, 1, file_size, archivator);
				}
			}

			fwrite(_buff, file_size, 1, file);
			free(_buff);
			search_flag = 0;
		}
		else
		{

			int counter = 0;
			while (counter != (file_size + 1))
			{
				counter += 1;
				ch = fgetc(archivator);
			}
			before_file_size = ftell(archivator);
		}
		free(cur_file_name);
	}

;
	char* file_before = 0, * file_after = 0;
	int length;

	fseek(archivator, 0, SEEK_END);
	length = ftell(archivator);
	fseek(archivator, 0, SEEK_SET);

	if (before_file_size != 0)
	{
		if (archivator)
		{
			file_before = malloc(before_file_size);
			if (file_before)
			{
				fread(file_before, 1, before_file_size, archivator);
			}
		}
		//		printf("\nPre buffer:\n|%s|\n", _buff);
	}

	//	printf("\nOdd chars:\n|");
	int counter = 0;
	while (counter != file_size + pars_len)
	{
		ch = fgetc(archivator);
		//		printf("%c", ch);
		counter += 1;
	}
	//	printf("|\n");

	//printf("%d\n", length - archive_size - counter);
	if (length != ftell(archivator))
	{
		if (archivator)
		{
			file_after = malloc(length - before_file_size - counter);
			if (file_after)
			{
				fread(file_after, 1, length - before_file_size - counter, archivator);
			}
		}
		//printf("\nAfter buffer:\n|%s|\n", _buff);
	}



	archivator = fopen(arch_name, "w");
	if (file_before)
		fwrite(file_before, before_file_size, 1, archivator);
	if (file_after)
		fwrite(file_after, length - before_file_size - counter, 1, archivator);


	fclose(file);
	fclose(archivator);
	free(file_before);
	free(file_after);

	struct stat rm_stat;
	stat(arch_name, &rm_stat);
	if (rm_stat.st_size == 0)
		remove(arch_name);
}

void Help()
{
	printf("\nHElP\nManual for archivator. There are 4 flags for archivator:\n");
	printf(" -h help flag. Prints manual for work with archivator\n");
	printf(" -s stat flag. Prints stat info for current archive (./archivator arch_name.txt -s)\n");
	printf(" -i insert flag. Inserts file into archive (./archivator arch_name.txt -i file.txt)\n");
	printf(" -e extract flag. Extracts file from archive (./archivator arch_name.txt -e file.txt)\n");
}

void Stat_info(char* arch_name)
{
	printf("\nSTAT:\n");

	int file_amount = CountAmount(arch_name, " ", 0);
	printf("\nFile amount: %d\n", file_amount);

	struct stat buff;
	stat(arch_name, &buff);
	printf("Arch_name: %s\nSize: %ld\n", arch_name, buff.st_size);
	//printf("Number of files: %d\n", amount);

	//data
	printf("Last time archive was changed: ");
	time_t t = buff.st_ctime;
	struct tm ftime;
	time_t now;
	localtime_r(&t, &ftime);
	time(&now);

	switch (ftime.tm_mon)
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

	if (localtime(&now)->tm_year == localtime(&buff.st_ctime)->tm_year)
	{
		printf(" %d %02d:%02d\n", ftime.tm_mday, ftime.tm_hour, ftime.tm_min);
	}
	else
	{
		printf(" %d %d\n", ftime.tm_mday, 1900 + ftime.tm_year);
	}
}

int main(int argc, char* argv[])
{
	int go;
	char cur_flag = ' ';
	char* arch, * file;
	arch = argv[1];
	if (arch[0] == '-')
	{
		printf("First paramets must be archive name");
		return 3;
	}


	while ((go = getopt(argc, argv, "i:e:hs")) != -1)
	{
		switch (go)
		{
		case 'i':
			insertion(arch, optarg);
			break;
		case 'e':
			extraction(arch, optarg);
			break;
		case 'h':
			Help();
			break;
		case 's':
			Stat_info(arch);
			break;
		case '?':
			printf("ERROR");
			return 1;
		default:
			printf("NO FLAG");
			return 2;
		}
	}

	return 0;
}
