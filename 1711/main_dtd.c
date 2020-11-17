#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include "headers/dtd.h"
#include <malloc.h>

dtd_t* dtd = NULL;

bool f(){
		FILE* fichier = fopen("dtd.dtd", "r");
	if(!fichier){
		fprintf(stderr, "%s\n", strerror(errno));
	}

	dtd = checkDtd(fichier);
	dtdPrint(dtd);
	destroyDtd(&dtd);

	fclose(fichier);
	return true;
}

int main(){
	f();

 	return EXIT_SUCCESS;
}
