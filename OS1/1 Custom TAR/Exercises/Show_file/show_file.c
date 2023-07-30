#include <stdio.h>
#include <stdlib.h>
#include <err.h>

int main(int argc, char* argv[]) {
	FILE* file=NULL;

	if (argc!=3) {
		fprintf(stderr,"Usage: %s <file_name> <block_size>\n",argv[0]);
		exit(1);
	}

	char *where;
	where = NULL;

	int size = atoi(argv[2]);
	int blocks = 0;

	/* Open file */
	if ((file = fopen(argv[1], "r")) == NULL)
		err(2,"The input file %s could not be opened",argv[1]);

	while(fread(&where, size, 1, file) == size){
		blocks++;
		if(!feof(file))fwrite(&where, size, 1, stdout);
	}
	
	fseek(file, blocks*size, SEEK_SET);
	while(!feof(file)){
		fread(&where, 1, 1, file);
		if(!feof(file)) fwrite(&where, 1, 1, stdout);
	}

	
	fclose(file);
	return 0;
}
