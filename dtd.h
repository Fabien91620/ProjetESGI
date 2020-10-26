#ifndef __DTD_H__
#define __DTD_H__

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

typedef struct element element_t;
typedef struct dtd dtd_t;

struct element{
	char name;
	element_t* fils;
	element_t* frere;
};

struct dtd{
	char* name;
	element_t* premier_fils;
};

bool check_type(FILE* fichier);
bool check_one_space(FILE* fichier);
bool check_name(FILE* fichier, dtd_t* dtd_entree);

#endif
