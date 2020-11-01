#include "dtd.h"


///////////// les 3 fonctions suivantes sont à revoir ///////////////////////////////


bool check_element_parametre(FILE* fichier, element_t* e){
	if(!fichier) return false;
	if(!e) return false;
	int charactere = fgetc(fichier);
	if(charactere != (int)'(') return false;
	charactere = fgetc(fichier);
	char chaine[100] = {0};
	int taille = 0;
	// Accepter uniquement  [a-z]  &&  [A-Z]  &&  trier de 6  &&  tiret de 8
	while(charactere != EOF && ((charactere >= (int)'a' && charactere <= (int)'z') || (charactere >= (int)'A' && charactere <= (int)'Z') || charactere == (int)'-'|| charactere == (int)'_' || charactere == (int)'#')) {
		if(taille && charactere==(int)'#') return false; // # ne peut se trouver quau début et que accompagné de PCDATA
		chaine[taille] = (char)charactere;
		taille++;
		charactere = fgetc(fichier);	
	}
	if(!strcmp(chaine, e -> name)) return false;
	if(charactere == (int)')'){
		if(!strcmp(chaine, "#PCDATA")){
			e -> type = PCDATA;
		}else{
			if(chaine[0]==(int)'#') return false;
			e -> type = SOUS_ELEMENT;
			e -> parametres.type = UN;
			e -> parametres.name = malloc(sizeof(char) * (taille+1) );
			strcpy(e -> parametres.name, chaine);
		}
		return true;
	}
	if(chaine[0]==(int)'#')return false;	
	e -> parametres.name = malloc(sizeof(char) * (taille+1) );
	strcpy(e -> parametres.name, chaine);
	if(charactere == (int)'+') {
		e -> type = SOUS_ELEMENT;
		e -> parametres.type = UN_OU_PLUS;
		return (fgetc(fichier)==(int)')');
	}
	if(charactere == (int)'*'){
		e -> type = SOUS_ELEMENT;
		e -> parametres.type = ZERO_OU_PLUS;
		return (fgetc(fichier)==(int)')');
	}
	if(charactere == (int)'?'){
		e -> type = SOUS_ELEMENT;
		e -> parametres.type = ZERO_OU_UN;
		return (fgetc(fichier)==(int)')');
	}
	free(e -> parametres.name);
	return false;
}


void affiche_DTD(dtd_t* dtd){
	if(!dtd)return;
	printf("|%s|\n", dtd -> name);
	if(!dtd -> premier_element) return;
	element_t* e = dtd -> premier_element;
	element_t* f;

	while(e){
		f = e -> premier_fils;
		printf("<!ELEMENT %s ", e -> name);
		if(e -> type == PCDATA){
			printf("(%s)>\n", "#PCDATA");
		}else{
			printf("(%d,", e -> type);
			printf(" %s,", e -> parametres.name);
			printf(" %d)>\n", e -> parametres.type);	
		}
		while(f){
			printf("\t<!ELEMENT FILS %s ", f -> name);
			if(f -> type == PCDATA){
				printf("(%s)>\n", "#PCDATA");
			}else{
				printf("(%d,", f -> type);
				printf(" %s,", f -> parametres.name);
				printf(" %d)>\n", f -> parametres.type);
			}
			f =  f -> element_suivant;
		}		
		e = e -> element_suivant;
	}
}
///////////// les 3 fonctions au dessus à revoir ///////////////////////////////













void skip_spaces(FILE* fichier, int space){
	if(!fichier) return;
	int espace = fgetc(fichier);
	// Ignorer tous les espaces ' ', '\t', '\n'
	if(space == (int)'*'){
		while(espace == (int)' ' || espace == (int)'\n' || espace == (int)'	'){
			espace = fgetc(fichier);
		}
	// Ignorer des charactères spécifiques
	}else {
		while(espace == space){
			espace = fgetc(fichier);
		}
	}
	fseek(fichier, -1, SEEK_CUR);
}


void add_element_child(element_t* des, element_t* src){
	if(!des || !src) return;
	if(!des -> premier_fils) {
		des -> premier_fils = src;
		return;
	}
	element_t* itterator = des -> premier_fils;
	while(itterator -> element_suivant){
		itterator = itterator -> element_suivant;
	}
	itterator -> element_suivant = src;
}


bool serach_for_child(dtd_t* dtd, element_t* e){
	if(!dtd || !dtd -> premier_element) return false;
	if(!e) return false;
	element_t* itterator = dtd -> premier_element;
	if(itterator != e && !strcmp(itterator -> name, e -> parametres.name)){	
		dtd -> premier_element = dtd -> premier_element -> element_suivant;
		itterator -> element_suivant = NULL;

		add_element_child(e, itterator);
		return true;
	}
	while(itterator -> element_suivant){
		if(itterator -> element_suivant != e){

			if(!strcmp(itterator -> element_suivant -> name, e -> parametres.name)){
				element_t * save = itterator;
				itterator = itterator -> element_suivant;
				save -> element_suivant = save -> element_suivant -> element_suivant;
				
				itterator -> element_suivant = NULL;
				add_element_child(e, itterator);
				return true;
			}
		}
		itterator = itterator -> element_suivant;

	}
	return false;
}


void add_element(dtd_t* dtd, element_t* e){
	if(!dtd) return;
	if(!e) return;
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


bool check_element_name(FILE* fichier, element_t* e){
	if(!fichier) return false;
	if(!e) return false;
	char chaine[100] = {0};
	int taille = 0;
	int charactere = fgetc(fichier);
	// Accepter uniquement  [a-z]  &&  [A-Z]  &&  trier de 6  &&  tiret de 8
	while(charactere != EOF && ((charactere >= (int)'a' && charactere <= (int)'z') || (charactere >= (int)'A' && charactere <= (int)'Z') || charactere == (int)'-'|| charactere == (int)'_' )){		
		chaine[taille] = (char)charactere;
		taille++;
		charactere = fgetc(fichier);
	}
	if(charactere == EOF) return false;
	if(charactere != (int)' ')return false;
	if(fgetc(fichier) != (int)'(') return false;
	fseek(fichier, -1, SEEK_CUR);
	if(taille<=0) return false;
	e -> name = (char*)malloc(sizeof(char) * (strlen(chaine)+1));
	if(!e -> name) ERROR_ALLOCATION;
	strcpy(e -> name, chaine);
	return true;
}


bool check_element_type_ELEMENT(FILE* fichier){
	if(!fichier) return false;
	char chaine[8] = {0};
	fgets(chaine, 8, fichier);
	if(strcmp(chaine,"ELEMENT")) return false;
	if(fgetc(fichier)!=(int)' ') return false; // espace obligatoire
	return true;
}


element_t* check_element(FILE* fichier){
	if(!fichier) return NULL;
	if(fgetc(fichier)!= (int)'<'){
		fseek(fichier, -1, SEEK_CUR);
		return NULL;
	}
	if(fgetc(fichier) != (int)'!') return NULL;
	if(!check_element_type_ELEMENT(fichier)) return NULL;
	element_t* e = NULL;
	e = (element_t*)malloc(sizeof(element_t));
	if(!e) ERROR_ALLOCATION;
	e -> name = NULL;
	e -> premier_fils = NULL;
	e -> element_suivant = NULL;

	if(!check_element_name(fichier, e)){
		free(e);
		return NULL;
	}
	if(!check_element_parametre(fichier, e)){
		free(e -> name);
		free(e);
		return NULL;
	}
	if(fgetc(fichier) != (int)'>'){
		free(e -> name);
		//liberer les paramteres
		free(e);
		return NULL;
	}
	if(fgetc(fichier) != (int)'\n'){
		free(e -> name);
		//liberer les paramteres
		free(e);
		return NULL;
	}
	return e;
}


bool check_dtd_racine(FILE* fichier, dtd_t* dtd){
	if(!fichier) return false;
	char chaine[100] = {0};
	int taille = 0;
	int charactere = fgetc(fichier);
	// Accepter uniquement  [a-z]  &&  [A-Z]  &&  trier de 6  &&  tiret de 8
	while(charactere != EOF && ((charactere >= (int)'a' && charactere <= (int)'z') || (charactere >= (int)'A' && charactere <= (int)'Z') || charactere == (int)'-'|| charactere == (int)'_' )){		
		chaine[taille] = (char)charactere;
		taille++;
		charactere = fgetc(fichier);
	}
	if(charactere == EOF) return false;
	dtd -> name = calloc(taille, sizeof(char));
	strcpy(dtd -> name, chaine);
	return true;
}


bool check_dtd_DOCTYPE(FILE* fichier){
	if(!fichier) return false;
	char chaine[8] = {0};
	fgets(chaine, 8, fichier);
	if(strcmp(chaine,"DOCTYPE")) return false;
	if(fgetc(fichier)!=(int)' ') return false;
	return true;
}


bool check_dtd(FILE* fichier, dtd_t* dtd){
	if(!fichier) return false;
	if(!dtd) return false;
	if(fgetc(fichier) != (int)'<') return false;
	if(fgetc(fichier) != (int)'!') return false;
	if(!check_dtd_DOCTYPE(fichier)) return false;
	if(!check_dtd_racine(fichier, dtd)) return false;
	if(fgetc(fichier) != (int)'[') return false;
	if(fgetc(fichier) != (int)'\n') return false;
	element_t* e = check_element(fichier);
	while(e){
		add_element(dtd, e);
		e = check_element(fichier);
	}	
	if(fgetc(fichier) != (int)']') return false;
	if(fgetc(fichier) != (int)'>') return false;
	// Suppression des lignes et des espaces apres le fichier

	int charactere = fgetc(fichier);

	while(charactere != EOF){

		if(charactere != (int)' ' && charactere != (int)'\t' && charactere != (int)'\n') return false;
		charactere = fgetc(fichier);
	}
	element_t* itterator = dtd -> premier_element;
	while(itterator){
		if(itterator -> type == SOUS_ELEMENT){
			serach_for_child(dtd, itterator);
		}
		itterator = itterator -> element_suivant;
	}
	return true;
}