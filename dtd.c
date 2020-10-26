#include "dtd.h"


bool check_type(FILE* fichier){
	if(!fichier) return false;
	if(fgetc(fichier) != (int)'<') return false;
	if(fgetc(fichier) != (int)'!') return false;
	char chaine[100];
	fgets(chaine, 8, fichier);
	if(strcmp(chaine,"DOCTYPE")) return false;
	return true;
}

bool check_one_space(FILE* fichier){
	if(!fichier) return false;
	if(fgetc(fichier) != (int)' ') return false;
	if(fgetc(fichier) == (int)' ') return false;
	fseek(fichier, -1,SEEK_CUR);
	return true;
}

bool check_name(FILE* fichier, dtd_t* dtd_entree){
	if(!fichier) return false;
	char chaine[100] = {0};
	chaine[0] = (char)0;
	int taille = 0;
	int charactere = fgetc(fichier);

	if(charactere == EOF || charactere == (int)'[') return false;
	while(charactere != (int)'['){
		if(charactere != (int)' '){
			chaine[taille] = (char)charactere;
			taille++;
		}
		charactere = fgetc(fichier);
	}
	// chaine[taille] = 0;// Pas obligatoire (voir ligne 24)
	dtd_entree -> name = calloc(taille,sizeof(char));
	strcpy(dtd_entree -> name, chaine);
	return true;
}
