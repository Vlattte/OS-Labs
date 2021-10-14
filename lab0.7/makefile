all: build

build:
	gcc archivator.c -o archivator	
	./archivator arch -i file1 -i file1 
clean:
	rm archivator
	rm arch
extraction_all:
	./archivator arch -i archivator.c -e archivator.c -i file1 -i file2 -e file1 -i archivator.c -i file1 -i file1
extr_first:
	 ./archivator arch -e file1
extr_last:
	./archivator arch -e file2
insert_three:
	./archivator arch -h -s -i file1 -i file2 -i file3
