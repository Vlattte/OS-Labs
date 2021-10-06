#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

//move file to arch_file
void insertion(char* arch_name, char* file_name)
{
	FILE* file, *archivator;
	FILE* count_amount;
	int alrdy_exts = 0;

	if ((file = fopen(file_name, "r")) == NULL)
	{
		printf("Cannot open the file %s\n", file_name);
		exit(2);
	}
	if (access(arch_name, 0) == -1)
		alrdy_exts = 1;
	archivator = fopen(arch_name, "a");
	int amount = 0;

	if ((access("CountAmount", 0) == 0) && (alrdy_exts != 1))
	{
		count_amount = fopen("CountAmount", "r+");
		fscanf(count_amount, "%d", &amount);
		fclose(count_amount);
	}

	count_amount = fopen("CountAmount", "w");
	amount += 1;
	fprintf(count_amount, "%d", amount);
	fclose(count_amount);

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
	fprintf(archivator, "%s %d\n", file_name, length);
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


	
	char buff[strlen(arch_name)];
	char ch;
	fpos_t pos;
	long name_len = 0;
	long name = 0;
	int search_flag = 1;

	int archive_size = 0;
	int file_size;

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
			//printf("|%c|", ch);	
			if (ch == ' ')
				name = name_len;;
			name_len += 1;
		}

		fsetpos(archivator, &pos);
		fscanf(archivator, "%s %d", buff, &file_size);

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

	FILE* narch;
	int length;
	narch = fopen("NEW_ARCH", "w");

	fseek(archivator, 0, SEEK_END);
	length = ftell(archivator);
	fseek(archivator, 0, SEEK_SET);

	if (archive_size != 0)
	{
		char* _buff = 0;
		if (archivator)
		{
			_buff = malloc(archive_size + 1);
			if (_buff)
			{
				fread(_buff, 1, archive_size, archivator);
			}
		}
		printf("\nPre buffer:\n|%s|\n", _buff);
		fprintf(narch, _buff);
		free(_buff);
	}

	printf("\nOdd chars:\n|");
	int counter = 0;
	while (counter != file_size + name_len)
	{
		ch = fgetc(archivator);
		printf("%c", ch);
		counter += 1;
	}
	printf("|\n");

	printf("length%d\n archive_size%d\n counter%d\n", length, archive_size, counter);
	printf("%d\n", length - archive_size - counter);
	if (length != ftell(archivator))
	{
		char* _buff = 0;
		if (archivator)
		{
			_buff = malloc(length - archive_size - counter);
			if (_buff)
			{
				fread(_buff, 1, length - archive_size - counter, archivator);
			}
		}
		fprintf(narch, _buff);
		printf("\nAfter buffer:\n|%s|\n", _buff);
		free(_buff);
	}

	//delete old archive
	//rename new archive into new one
	//fprintf(narch, "\n");

	fclose(narch);
	if(archivator)
		fclose(archivator);
	fclose(file);

	remove(arch_name);
	rename("NEW_ARCH", arch_name);
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
			printf("it's input flag\n");
			insertion(arch, optarg);
			break;
		case 'e':
			printf("it's extract flag\n");
			extraction(arch, optarg);
			break;
		case 'h':
			printf("HELP ME\n");
			break;
		case 's':
			printf("STATATTADSTADYASDGSHDSAJHd\n");
			break;
		case '?':
			printf("ERROR");
			return 1;
		}
	}

	if (cur_flag == 'i')
		insertion(arch, file);
	else if (cur_flag == 'e')
		extraction(arch, file);

	return 0;
}
