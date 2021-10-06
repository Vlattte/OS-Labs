all: build

build:
	gcc archivator.c -o archivator	
	./archivator arch -i file1 -i file2 -e file1 -h -s
clean:
	rm archivator
	rm arch.txt
extraction_all:
	./archivator arch -e file1 -e file2
extr_first:
	 ./archivator arch -e file1
extr_last:
	./archivator arch -e file2
insert_three:
	./archivator arch -h -s -i file1 -i file2 -i file3
