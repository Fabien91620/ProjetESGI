#include "dtd.h"



void skip_spaces(FILE* fichier){
	int espace = fgetc(fichier);
	while(espace == (int)' '){
		espace = fgetc(fichier);
	}
	fseek(fichier, -1, SEEK_CUR);
}

void skip_lines(FILE* fichier){
	int espace = fgetc(fichier);
	while(espace == (int)'\n'){
		espace = fgetc(fichier);
	}
	fseek(fichier, -1, SEEK_CUR);
}

void skip_tab(FILE* fichier){
	int espace = fgetc(fichier);
	while(espace == (int)'	'){
		espace = fgetc(fichier);
	}
	fseek(fichier, -1, SEEK_CUR);
}

void skip_spaces_lines_tab(FILE* fichier){
	int espace = fgetc(fichier);
	while(espace == (int)' ' || espace == (int)'\n' || espace == (int)'	'){
		espace = fgetc(fichier);
	}
	fseek(fichier, -1, SEEK_CUR);
}


// bool check_one_space(FILE* fichier){
// 	if(!fichier) return false;
// 	if(fgetc(fichier) != (int)' ') return false;
// 	if(fgetc(fichier) == (int)' ') return false;
// 	fseek(fichier, -1,SEEK_CUR);
// 	return true;
// }


// Verification du type du DTD "DOCTYPE"
bool check_dtd_type(FILE* fichier){
	char chaine[8] = {0};
	fgets(chaine, 8, fichier);
	if(strcmp(chaine,"DOCTYPE")) return false;
	if(fgetc(fichier)!=(int)' ') return false; // espace obligatoire
	// Supprimer d'eventuels espaces
	return true;
}

// Verification du nom du DTD "ESGI"
bool check_dtd_name(FILE* fichier, dtd_t* dtd){
	if(!fichier) return false;
	char chaine[100] = {0};
	int taille = 0;
	int charactere = fgetc(fichier);

	while(charactere != EOF && charactere != (int)'[' && charactere != (int)' '){
		chaine[taille] = (char)charactere;
		taille++;
		charactere = fgetc(fichier);
	}
	if(charactere == EOF) return false;
	if(charactere == (int)' '){
		// Supprimer d'eventuels espaces
		if(fgetc(fichier) != (int)'[') return false;
		// Supprimer d'eventuels espaces ou tabulations
		// if(fgetc(fichier) != (int)'\n')return false; pas obligatoire d'après ma conception
	}
	dtd -> name = calloc(taille, sizeof(char));
	strcpy(dtd -> name, chaine);
	return true;
}


// Verification du type d'un element "ELEMENT"
bool check_element_type(FILE* fichier){
	if(!fichier) return false;
	// Supprimer d'eventuels espaces
	char chaine[8] = {0};
	fgets(chaine, 8, fichier);
	if(strcmp(chaine,"ELEMENT")) return false;
	if(fgetc(fichier)!=(int)' ') return false; // espace obligatoire
	return true;
}

// Verification du nom d'un element "classrooms, classroom..."
bool check_element_name(FILE* fichier){
	char chaine[100] = {0};
	int taille = 0;
	int charactere = fgetc(fichier);
	while(charactere != EOF  && charactere != (int)' ' && charactere != (int)'('){		
		chaine[taille] = (char)charactere;
		taille++;
		charactere = fgetc(fichier);
	}
	if(charactere == EOF) return false;
	if(charactere == (int)' '){
		// Supprimer d'eventuels espaces
		if(fgetc(fichier) != (int)'(') return false;
	}
	if(strlen(chaine)<=0) return false;
	fseek(fichier, -1, SEEK_CUR);
	return true;
}

// Verification des parametres (un seul parametre est pris en compte pour l'instant) d'un element "classroom..." et le nombre de parametres + pour 1 ou plus, * pour 0 ou plus, rien pour exactement 1
bool check_element_parameter(FILE* fichier){
	if(!fichier) return false;
	int charactere = fgetc(fichier);
	if(charactere != (int)'(') return false;
	charactere = fgetc(fichier);
	char chaine[100] = {0};
	int taille = 0;
	while(charactere != EOF && charactere != (int)')' && charactere != (int)'+' && charactere != (int)'*'){
		chaine[taille] = (char)charactere;
		taille++;
		charactere = fgetc(fichier);
	}
	if(charactere == (int)')'){
		// juste 1
		return true;
	}
	if(charactere == (int)'+') {
		// au moins 1 au plus l'infini
		//Supprimer d'evetuels espaces
		return (fgetc(fichier)==(int)')');
	}
	if(charactere == (int)'*'){
		// 0 ou l'infini
		//Supprimer d'evetuels espaces
		return (fgetc(fichier)==(int)')');
	}

	// il faut mettre les parametres dans la structure de l'element
	return false;	
}


// Fonction qui fait appel aux fonctions au dessus
bool check_element(FILE* fichier, dtd_t* dtd){
	if(!fichier) return false;

		skip_spaces_lines_tab(fichier);

	if(fgetc(fichier) != (int)'<'){
		fseek(fichier, -1, SEEK_CUR);
		return false;
	}	
	if(fgetc(fichier) != (int)'!') return false;
	
	printf("%s\n", check_element_type(fichier)?"element type OK\n":"ERROR\n");

	printf("%s\n", check_element_name(fichier)?"element name OK\n":"ERROR\n");

	printf("%s\n", check_element_parameter(fichier)?"element parameter OK\n":"ERROR\n");


	if(fgetc(fichier) != (int)'>') return false;
	skip_spaces_lines_tab(fichier);

	return true;
}



bool check_dtd(FILE* fichier, dtd_t* dtd){
	if(!fichier) return false;
	// supprimer les eventuels espaces
	if(fgetc(fichier) != (int)'<') return false;

	// supprimer les eventuels espaces
	if(fgetc(fichier) != (int)'!') return false;
	// supprimer les eventuels espaces
	if(!check_dtd_type(fichier)) return false;

	// supprimer les eventuels espaces
	if(!check_dtd_name(fichier, dtd)) return false;

		// supprimer les eventuels espaces

	bool read_element = check_element(fichier, dtd); 
	
	while(read_element){
		read_element = check_element(fichier, dtd);
			
	}
	if(fgetc(fichier) != (int)']') return false;
	if(fgetc(fichier) != (int)'>') return false;

	// Tout est OK
	return true;
}




// il faut désormais enregistrer les elements dans la structure de données
	

// à ajouter :
	// une fonction qui n'arrete pas de parcourir le fichier tant que il n y'a
	// que des espaces, des tabulatures ou des sauts de ligne : pour supprimer les
	// espaces entre deux mots ou deux characteres


// Je dois restructurer le code en factorisant les fonctions