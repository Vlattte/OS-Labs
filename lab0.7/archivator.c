#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

//move file to arch_file
void insertion(char* arch_name, char* file_name)
{
	FILE* file, *archivator;
	//FILE* count_amount;
	int alrdy_exts = 0;
	
	struct stat file_stat;
	stat(file_name, &file_stat);
	mode_t mode = file_stat.st_mode;

	if ((file = fopen(file_name, "r")) == NULL)
	{
		printf("Cannot open the file %s\n", file_name);
		exit(2);
	}
	if (access(arch_name, 0) == -1)
		alrdy_exts = 1;
	archivator = fopen(arch_name, "a+");
	//int amount = 0;

	/*if ((access("CountAmount", 0) == 0) && (alrdy_exts != 1))
	{
		count_amount = fopen("CountAmount", "r+");
		fscanf(count_amount, "%d", &amount);
		fclose(count_amount);
	}

	count_amount = fopen("CountAmount", "w");
	amount += 1;
	fprintf(count_amount, "%d", amount);
	fclose(count_amount);*/

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
	fprintf(archivator, "%s %d %d\n", file_name, mode, length);
	fprintf(archivator, buff);

	free(buff);
	fclose(file);
	fclose(archivator);
}


//extract current file_name from current archive
void extraction(char* arch_name, char* file_name)
{
	FILE* file, * archivator;
	file = fopen(file_name, "w");
	if ((archivator = fopen(arch_name, "r")) == NULL)
	{
		printf("Cannot open the file %s\n", arch_name);
		exit(2);
	}

	/*
	FILE* count_amount;
	count_amount = fopen("CountAmount", "r+");
	int amount = 0;

	fscanf(count_amount, "%d", &amount);
	amount -= 1;
	fclose(count_amount);
	if (amount == 0)
		remove("CountAmount");
	else
	{
		count_amount = fopen("CountAmount", "w");
		fprintf(count_amount, "%d", amount);
		fclose(count_amount);
	}
	*/

	char buff[strlen(arch_name)];
	char ch;
	fpos_t pos;
	long name_len;
	long name;
	int search_flag = 1;

	int archive_size = 0;
	int file_size;
	int mode_len = 0;

	while (search_flag)
	{
		file_size = 0;
		name_len = 0;
		name = 0;

		//name_len
		fgetpos(archivator, &pos);
		ch = ' ';
		while (ch != '\n')
		{
			ch = fgetc(archivator);
			if (ch == ' ' && name != 0)
				name = name_len;
			else
				mode_len = name_len;
			name_len += 1;
		}

		fsetpos(archivator, &pos);
		fscanf(archivator, "%s %d %d", buff, &mode_len, &file_size);

		if (strcmp(buff, file_name) == 0)
		{
			char* _buff = 0;

			if (archivator)
			{
				_buff = malloc(file_size);
				if (_buff)
				{
					ch = fgetc(archivator);
					fread(_buff, 1, file_size, archivator);
				}
			}
			fprintf(file, _buff);
			if (_buff)
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
			archive_size += name_len + file_size;
		}
	}

	char* file_before = 0, * file_after = 0;
	int length;

	fseek(archivator, 0, SEEK_END);
	length = ftell(archivator);
	fseek(archivator, 0, SEEK_SET);

	if (archive_size != 0)
	{
		if (archivator)
		{
			file_before = malloc(archive_size + 1);
			if (file_before)
			{
				fread(file_before, 1, archive_size, archivator);
			}
		}
//		printf("\nPre buffer:\n|%s|\n", _buff);
	}

//	printf("\nOdd chars:\n|");
	int counter = 0;
	while (counter != file_size + name_len)
	{
		ch = fgetc(archivator);
//		printf("%c", ch);
		counter += 1;
	}
//	printf("|\n");

//	printf("%d\n", length - archive_size - counter);
	if (length != ftell(archivator))
	{
		if (archivator)
		{
			file_after = malloc(length - archive_size - counter);
			if (file_after)
			{
				fread(file_after, 1, length - archive_size - counter, archivator);
			}
		}
//		printf("\nAfter buffer:\n|%s|\n", _buff);
	}

	if(archivator)
		fclose(archivator);
	fclose(file);

	archivator = fopen(arch_name, "w");
	//printf("Before:\n%s\n\nAfter:\n%s\n");
	if(file_before)
		fprintf(archivator, "%s", file_before);
	if(file_after)
		fprintf(archivator, "%s", file_after);
	//fprintf(archivator, file_after);
        free(file_after);
        free(file_before);
	fclose(archivator);
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
	/*
	FILE* count_amount;
	if ((count_amount = fopen("CountAmount", "r+")) == NULL)
	{
		printf("No files in archive = no stat\n");
		return;
	}

	int amount = 0;
	fscanf(count_amount, "%d\n", &amount);
	fclose(count_amount);

	if (amount == 0)
	{
		printf("No files in archive = no stat\n");
		return;
	}
	*/

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
		}
	}

	return 0;
}
