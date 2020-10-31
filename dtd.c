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


bool check_exist_space(FILE* fichier){
	if(!fichier) return false;
	if(fgetc(fichier) != (int)' ') return false;
	return true;
}

void add_element(dtd_t* dtd, element_t* e){
	if(!dtd -> premier_element){
		dtd -> premier_element = e;
		return;
	}
	element_t* itterator = dtd -> premier_element;
	while(itterator -> element_suivant){

		itterator = itterator -> element_suivant;
	}
	itterator -> element_suivant = e;
}

void add_paramatre(element_t* e, char* parametre, char signe){
	if(!e) return;
	if(!e -> parametre.nomParametreArborescent){

		e -> parametre.nomParametreArborescent = malloc(sizeof(nomParametreArborescent_t));
		e -> parametre.nomParametreArborescent -> name = malloc(sizeof(char) * (strlen(parametre)+1));
		strcpy(e -> parametre.nomParametreArborescent -> name, parametre);
		
		switch(signe){
			case '+':	e -> parametre.nomParametreArborescent -> nombreDeFils = UN_OU_PLUS;
						break;
			case '*':	e -> parametre.nomParametreArborescent -> nombreDeFils = ZERO_OU_PLUS;
						break;
			case ' ':	e -> parametre.nomParametreArborescent -> nombreDeFils = UN;
						break;
			default:
						return;					
		}

		e -> parametre.nomParametreArborescent -> suivant = NULL;
	}
}


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
bool check_element_type_ELEMENT(FILE* fichier){
	if(!fichier) return false;
	// Supprimer d'eventuels espaces
	char chaine[8] = {0};
	fgets(chaine, 8, fichier);
	if(strcmp(chaine,"ELEMENT")) return false;
	if(fgetc(fichier)!=(int)' ') return false; // espace obligatoire
	return true;
}

// Verification du nom d'un element "classrooms, classroom..."
bool check_element_name(FILE* fichier, element_t* e){
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

	e -> name = malloc(sizeof(char) * (strlen(chaine)+1));
	strcpy(e -> name, chaine);
	return true;
}

// Verification des parametres (un seul parametre est pris en compte pour l'instant) d'un element "classroom..." et le nombre de parametres + pour 1 ou plus, * pour 0 ou plus, rien pour exactement 1
bool check_element_parameter(FILE* fichier, element_t* e){
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
		if(!strcmp(chaine, "#PCDATA")){
			e -> type = SIMPLE;
			e -> parametre.parametreSimple = PCDATA;
		}else if(!strcmp(chaine, "#CDDATA")){
			e -> type = SIMPLE;
			e -> parametre.parametreSimple = CDDATA;
		}else{
			e -> type = ARBORESCENT;
			add_paramatre(e, chaine, ' ');
			
		}
		return true;
	}
	if(charactere == (int)'+') {
		e -> type = ARBORESCENT;
		add_paramatre(e, chaine, '+');
		//Supprimer d'evetuels espaces
		return (fgetc(fichier)==(int)')');
	}
	if(charactere == (int)'*'){
		e -> type = ARBORESCENT;
		add_paramatre(e, chaine, '*');
		//Supprimer d'evetuels espaces
		return (fgetc(fichier)==(int)')');
	}
	return false;	
}


// Fonction qui fait appel aux fonctions au dessus
element_t* check_element(FILE* fichier, dtd_t* dtd){
	if(!fichier) return NULL;

		skip_spaces_lines_tab(fichier);

	if(fgetc(fichier) != (int)'<'){
		fseek(fichier, -1, SEEK_CUR);
		return NULL;
	}
	skip_spaces(fichier);

	if(fgetc(fichier) != (int)'!') return NULL;
	
	skip_spaces(fichier);	

	
	printf("%s\n", check_element_type_ELEMENT(fichier)?"element type OK\n":"ERROR\n");

	element_t* e = malloc(sizeof(element_t));

	if(!check_element_name(fichier, e)){
		free(e);
		return NULL;
	}

	if(!check_element_parameter(fichier, e)){
		free(e);
		return NULL;
	}

	e -> element_suivant = NULL;

	if(fgetc(fichier) != (int)'>'){
		free(e);
		return NULL;
	}
	skip_spaces_lines_tab(fichier);

	return e;
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

	element_t* read_element = check_element(fichier, dtd); 
	
	while(read_element){
		add_element(dtd, read_element);
		read_element = check_element(fichier, dtd);
	}
	if(fgetc(fichier) != (int)']') return false;
	if(fgetc(fichier) != (int)'>') return false;

	// Tout est OK
	return true;
}






void affiche_DTD(dtd_t* dtd){
	if(!dtd)return;
	printf("|%s|\n", dtd -> name);
	if(!dtd -> premier_element) return;
	element_t* e;
	e = dtd -> premier_element;
	while(e){

		printf("|%s|	", e -> name);
		if(e -> type == SIMPLE){
			printf("|%d|\n", e -> parametre.parametreSimple);
		}else{
			if(e -> parametre.nomParametreArborescent){
				printf("%s", e -> parametre.nomParametreArborescent -> name);
				char fils[100];
				if(e -> parametre.nomParametreArborescent -> nombreDeFils==UN){
					strcpy(fils,"   : Un fils exactement");
				}
				else if(e -> parametre.nomParametreArborescent -> nombreDeFils == UN_OU_PLUS){
					strcpy(fils,"   : Un fils ou plusieurs mais jamais 0 --> +");
				}else{
					strcpy(fils,"   : Zero ou plusieurs fils --> *");
				}

				printf("%s\n", fils);
			
			}
		}
		e = e -> element_suivant;
	}

}