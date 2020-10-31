#ifndef __DTD_H__
#define __DTD_H__

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

typedef struct parametreElement parametreElement_t;
typedef struct element element_t;
typedef struct dtd dtd_t;
typedef enum typeElement typeElement_t;
typedef enum typeParametreSimple typeParametreSimple_t;
typedef struct nomParametreArborescent nomParametreArborescent_t;

enum typeElement{
	SIMPLE,
	ARBORESCENT
};

enum typeParametreSimple{
	PCDATA,
	CDDATA,	//...
};
		
struct nomParametreArborescent{
	char* name;
	enum{
		UN, 	
		UN_OU_PLUS,		// +
		ZERO_OU_PLUS	// *
	}nombreDeFils;

	nomParametreArborescent_t* suivant;
};

struct element{
	char* name;
	typeElement_t type;

	union{
		typeParametreSimple_t parametreSimple;
		nomParametreArborescent_t* nomParametreArborescent;
	}parametre;

	element_t* element_suivant;
};


struct dtd{
	char* name;
	element_t* premier_element;
};


bool check_dtd(FILE* fichier, dtd_t* dtd);
void affiche_DTD(dtd_t* dtd);

#endif
