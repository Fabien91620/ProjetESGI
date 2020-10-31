#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "dtd.h"

int main(){
	FILE* fichier = fopen("dtd.dtd", "r");
	if(!fichier){
		printf("%s\n", "Erreur lors de l'ouverture du ficher !\nLe programme va s'arreter");
		exit(1);
	}

	dtd_t* dtd = malloc(sizeof(dtd_t));

	printf("%s\n", check_dtd(fichier, dtd)?"dtd type OK\n":"ERROR\n");



	fclose(fichier);

 	return EXIT_SUCCESS;
}
