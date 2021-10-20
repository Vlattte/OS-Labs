#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>


void print_bad_place(char ch)
{
	printf("ERROR IN REQUEST. %c FLAG IS NOT ON iT'S PLACE\n", ch);
	exit(1);
}

void Help()
{
	printf("How to use chmod:\n\tfirst flag  is user(-u), group(-g) or others(-o) flag\n\t");
	printf("second flag is add(-a) or remove(-r) flag\n\t");
	printf("third flag  is read(-r), write(-w) or execute(-e) flags\n");
	printf("If request is written wrong, it will be blocked and ended\n");
	printf("If request tries to remove rights which are not exists, there will not be an error\n");
}

int main(int argc, char* argv[])
{
	int go;
	int place_flag = 0;
	int perm_count = 0; //max 3 permissions to add
	char gou_perm = ' ';
	int is_plus = 1;
	mode_t fixer = 00000;


	while ((go = getopt(argc, argv, "uogarwxh")) != -1)
	{
		switch (go)
		{
		//HELP
		case 'h':
			Help();
			place_flag = -1;
			break;
		//FIRST FLAGS
		case 'g':
			if (place_flag == 0)
				gou_perm = 'g';
			else
				print_bad_place('g');

			place_flag = 1;
			break;
		case 'o':
			if (place_flag == 0)
				gou_perm = 'o';
			else
				print_bad_place('o');

			place_flag = 1;
			break;
		case 'u':
			if (place_flag == 0)
				gou_perm = 'u';
			else
				print_bad_place('u');

			place_flag = 1;
			break;
		//SECOND FLAGS and read
		case 'a':
			if (place_flag == 1)
				is_plus = 1;
			else
				print_bad_place('a');
			
			place_flag = 2;
			break;
		case 'r': 
			if (place_flag == 1)
			{
				is_plus = -1;
				place_flag += 1;
			}
			else if ( (place_flag > 1) && (place_flag % 9) != 0)
			{
				if (gou_perm == 'u')
					fixer += 00400;
				if (gou_perm == 'g')
					fixer += 00040;
				if (gou_perm == 'o')
					fixer += 00004;
				place_flag *= 9;
			}
			else
				print_bad_place('r');
			break;
		//THIRD FLAGS
		case 'w':
			if (place_flag > 1 && (place_flag % 11) != 0)
			{
				if (gou_perm == 'u')
					fixer += 00200;
				if (gou_perm == 'g')
					fixer += 00020;
				if (gou_perm == 'o')
					fixer += 00002;
			}
			else
				print_bad_place('w');

			place_flag *= 11;
			break;
		case 'x':
			if (place_flag > 1 && (place_flag % 13) != 0)
			{
				if (gou_perm == 'u')
					fixer += 00100;
				if (gou_perm == 'g')
					fixer += 00010;
				if (gou_perm == 'o')
					fixer += 00001;
			}
			else
				print_bad_place('x');

			place_flag *= 13;
			break;
		case '?':
			printf("ERROR");
			return 1;
		default:
			printf("NOT A FLAG");
			return 2;
		}
	}

	struct stat st;
	if (place_flag > 2)
	{
		if (argv[argc - 1] == NULL)
		{
			printf("ERROR IN REQUEST. NO FILE\n");
			return 3;
		}
		else if (access(argv[argc - 1], 0) == 0)
			stat(argv[argc - 1], &st);
		else
			printf("NO SUCH FILE %s\n", argv[argc - 1]);

		mode_t mode = st.st_mode;
		if (is_plus == 1)
			chmod(argv[argc - 1], fixer | mode);
		else if (is_plus == -1)
		{
				chmod(argv[argc - 1], (~fixer) & mode);
		}
	}
	else if (place_flag != -1)
		printf("ERROR IN REQUEST!!! NO PERMISSION'S ARGUMENTS TO CHANGE\n");

	return 0;
}

