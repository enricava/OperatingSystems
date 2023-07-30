//Enrique Cavanillas Puga

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "mytar.h"

extern char *use;

/** Copy nBytes bytes from the origin file to the destination file.
 *
 * origin: pointer to the FILE descriptor associated with the origin file
 * destination:  pointer to the FILE descriptor associated with the destination file
 * nBytes: number of bytes to copy
 *
 * Returns the number of bytes actually copied or -1 if an error occured.
 */
int
copynFile(FILE * origin, FILE * destination, int nBytes)
{
	
	for(int n = 0; n < nBytes; ++n){
		int byte = getc(origin);
		if(feof(origin)) return n;
		if(putc(byte, destination)==EOF) return -1;
	}
	return nBytes;
}

/** Loads a string from a file.
 *
 * file: pointer to the FILE descriptor 
 * 
 * The loadstr() function must allocate memory from the heap to store 
 * the contents of the string read from the FILE. 
 * Once the string has been properly built in memory, the function returns
 * the starting address of the string (pointer returned by malloc()) 
 * 
 * Returns: !=NULL if success, NULL if error
 */
char*
loadstr(FILE * file)
{
	int size = 0;
	while(getc(file) != '\0') size++;

	char* result = malloc(size);
	
	if(fseek(file, -(size+1), SEEK_CUR)!=0) return NULL;
	
	for(int i = 0; i <= size; i++){
		result[i] = getc(file);
	}

	return result;
}

/** Read tarball header and store it in memory.
 *
 * tarFile: pointer to the tarball's FILE descriptor 
 * nFiles: output parameter. Used to return the number 
 * of files stored in the tarball archive (first 4 bytes of the header).
 *
 * On success it returns the starting memory address of an array that stores 
 * the (name,size) pairs read from the tar file. Upon failure, the function returns NULL.
 */
stHeaderEntry*
readHeader(FILE * tarFile, int *nFiles)
{

	fread(nFiles, sizeof(*nFiles), 1, tarFile);
	printf("Archivos a extraer: %i \n", *nFiles);

	stHeaderEntry* header = NULL;
	header = malloc(sizeof(stHeaderEntry)*(*nFiles));

	printf("Header:\n");
	
	for(int i = 0; i < *nFiles; i++){
		header[i].name = loadstr(tarFile);
		unsigned int size;
		fread(&size, 4, 1, tarFile);
		header[i].size = size;
		printf("[%i] Archivo \"%s\": %u bytes\n", i, header[i].name, header[i].size); 
	}
	
	return header;
}

/** Creates a tarball archive 
 *
 * nfiles: number of files to be stored in the tarball
 * filenames: array with the path names of the files to be included in the tarball
 * tarname: name of the tarball archive
 * 
 * On success, it returns EXIT_SUCCESS; upon error it returns EXIT_FAILURE. 
 * (macros defined in stdlib.h).
 *
 * HINTS: First reserve room in the file to store the tarball header.
 * Move the file's position indicator to the data section (skip the header)
 * and dump the contents of the source files (one by one) in the tarball archive. 
 * At the same time, build the representation of the tarball header in memory.
 * Finally, rewind the file's position indicator, write the number of files as well as 
 * the (file name,file size) pairs in the tar archive.
 *
 * Important reminder: to calculate the room needed for the header, a simple sizeof 
 * of stHeaderEntry will not work. Bear in mind that, on disk, file names found in (name,size) 
 * pairs occupy strlen(name)+1 bytes.
 *
 */
int
createTar(int nFiles, char *fileNames[], char tarName[])
{
	printf("Creando tarfile \"%s\" \n", tarName);

	FILE* tarFile = fopen(tarName, "w");
	if(tarFile == NULL) return EXIT_FAILURE;
	
	int headerSize = 0;
	headerSize += sizeof(int);
	
	for (int i = 0; i < nFiles; i++){
		headerSize += strlen(fileNames[i])+1;
		headerSize += sizeof(unsigned int);
	}

	fseek(tarFile, headerSize, SEEK_SET); 
	
	int* fileSizes = malloc(sizeof(int)*nFiles);

	for (int i = 0; i < nFiles; i++){
		printf("Copiando archivo \"%s\" ... ", fileNames[i]);	
		
		FILE* origin = fopen(fileNames[i], "r");
		if(origin == NULL) return EXIT_FAILURE;
		
		fileSizes[i] = copynFile(origin, tarFile, INT_MAX);
		if(fileSizes[i] == -1) return EXIT_FAILURE;
		fclose(origin);
		
		printf("copiado \n");
	}
	
	printf("Copiando headers ... ");

	fseek(tarFile, 0, SEEK_SET);
	
	fwrite(&nFiles, sizeof(int), 1, tarFile);
	
	for (int i = 0; i < nFiles; i++){
		fwrite(fileNames[i], sizeof(char), strlen(fileNames[i])+1, tarFile);
		fwrite(& fileSizes[i], sizeof(int), 1, tarFile);
	}
	printf("Archivo creado con exito  \n");

	free(fileSizes);
	fclose(tarFile);
	
	return EXIT_SUCCESS;
}

/** Extract files stored in a tarball archive
 *
 * tarName: tarball's pathname
 *
 * On success, it returns EXIT_SUCCESS; upon error it returns EXIT_FAILURE. 
 * (macros defined in stdlib.h).
 *
 * HINTS: First load the tarball's header into memory.
 * After reading the header, the file position indicator will be located at the 
 * tarball's data section. By using information from the 
 * header --number of files and (file name, file size) pairs--, extract files 
 * stored in the data section of the tarball.
 *
 */
int
extractTar(char tarName[])
{
	printf("Extrayendo \"%s\" \n", tarName);

	FILE* tarFile = fopen(tarName, "r");
	
	if(tarFile == NULL) return EXIT_FAILURE;
	
	int nFiles;

	stHeaderEntry* header = readHeader(tarFile, &nFiles);
	
	if(header == NULL) return EXIT_FAILURE;

	for(int i = 0; i < nFiles; i++){
		printf("[%i] Extrayendo \"%s\" : %u bits ... ", i, header[i].name, header[i].size);

		FILE* target = fopen(header[i].name, "w");
		if(target == NULL) return EXIT_FAILURE;
		
		int flag = copynFile(tarFile, target, header[i].size);
		if(flag != header[i].size) return EXIT_FAILURE;
		
		fclose(target);
		free(header[i].name);
		
		printf("Extraccion con exito\n");
	}
	
	free(header);
	fclose(tarFile);
	return EXIT_SUCCESS;
}
