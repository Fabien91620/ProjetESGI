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
	
	dtd_t* dtd_entree = malloc(sizeof(dtd_t));


	printf("--%d--\n",check_type(fichier));
	printf("--%d--\n",check_one_space(fichier));
	printf("--%d--\n",check_name(fichier, dtd_entree));
	
	printf("%s\n", dtd_entree -> name);
	printf("%ld\n", strlen(dtd_entree -> name));



	free(dtd_entree -> name);
	free(dtd_entree);
	fclose(fichier);

	return 0;
}
