#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include "dtd.h"

int main(){
	FILE* fichier = fopen("dtd.dtd", "r");
	if(!fichier){
		fprintf(stderr, "%s\n", strerror(errno));
	}

	dtd_t* dtd = malloc(sizeof(dtd_t));

	printf("%s\n", check_dtd(fichier, dtd)?"DTD VALIDE\n":"DTD NON-VALIDE\n");
	affiche_DTD(dtd);

	fclose(fichier);

 	return EXIT_SUCCESS;
}
