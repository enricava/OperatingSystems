#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>


int my_system(const char *comando){
	pid_t pid;
	int retorno = 0;
	char* binaryPath = "/bin/bash";

	if ((pid = fork()) == -1 ){ 
		retorno = -1;
	}
	else if (pid == 0){ //proceso hijo duplicado
		printf("Pid: %d \n Ejecuta: '%s' \n ", getpid(), comando);
		execl(binaryPath, binaryPath, "-c", comando, NULL);//utilizamos proceso para la ejecucion de comando
		//por si falla execl
		//exit(-1);
	}
	else {
		while(pid != wait(&retorno));
		printf("Ha terminado el proceso hijo\n");
	}

	return retorno;
}

int main(int argc, char* argv[])
{
	if (argc!=2){
		fprintf(stderr, "Usage: %s <command>\n", argv[0]);
		exit(1);
	}

	return my_system(argv[1]);
}

