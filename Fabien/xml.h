#ifndef __XML_H__
#define __XML_H__

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "xml.h"

#define ERROR_ALLOCATION {printf("Erreur lors de l'allocation mémoire.\nLe programme va s'arreter.\n");exit(0);}


typedef struct element_xml elementXml_t;
typedef struct xml xml_t;
typedef enum typeElement typeElement_t;
typedef struct parametres parametres_t;

enum typeElement{
	PCDATA,	//	données textuelles de type (#PCDATA)
	SOUS_ELEMENT	//	un autre élément pourra servir de type
};


struct element_xml{
    char* name;
    typeElement_t type;
    char* data;
    elementXml_t* premier_fils;
    elementXml_t* premier_frere;
};

struct xml{
    char* version;
    elementXml_t* premier_fils;
};



char afficher_char(FILE* fichier);
void afficheXML(xml_t* xml_entree);

bool check_xml(FILE* fichier, xml_t* xml_entree);

#endif
