#ifndef __DTD_H__
#define __DTD_H__

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define ERROR_ALLOCATION {printf("Erreur lors de l'allocation mémoire.\nLe programme va s'arreter.\n");exit(0);}

typedef struct element element_t;
typedef struct dtd dtd_t;
typedef enum typeElement typeElement_t;
typedef struct parametres parametres_t;

enum typeElement{
	PCDATA,	//	données textuelles de type (#PCDATA)
	SOUS_ELEMENT	//	un autre élément pourra servir de type
};//Aucun type d'élément ne peut être déclaré plus d'une fois.
		
struct parametres{
	char* name;
	enum{
		UN,				// rien
		UN_OU_PLUS,		// +
		ZERO_OU_PLUS,	// *
		ZERO_OU_UN		// ?
	}type;
};

struct element{
	char* name;
	typeElement_t type;
	parametres_t parametres;
	element_t* premier_fils;
	element_t* element_suivant;
};


struct dtd{
	char* name;
	element_t* premier_element;
};


bool check_dtd(FILE* fichier, dtd_t* dtd);
void affiche_DTD(dtd_t* dtd);

#endif
