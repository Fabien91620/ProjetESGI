#ifndef __DTD_H__
#define __DTD_H__

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

typedef struct parametre parametre_t;
typedef struct element element_t;
typedef struct dtd dtd_t;


struct parametre{
	char name;
	enum{
		RIEN,
		PLUS,
		ETOILE
	}nombre;
};

struct element{
	char name;
	parametre_t* parametres;
	element_t* element_fils;
	element_t* element_suivant;
};

struct dtd{
	char* name;
	element_t* premier_element;
};

bool check_dtd(FILE* fichier, dtd_t* dtd);

#endif
