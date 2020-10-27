#ifndef xml_h
#define xml_h

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "dtd.h"

typedef struct element_xml elementXml_t;
typedef struct xml xml_t;

struct element_xml{
    char* name;
    elementXml_t* fils;
    elementXml_t* frere;
};

struct xml{
    char* version;
    elementXml_t* premier_fils;
};

bool check_xml(FILE* fichier);
bool check_one_space_xml(FILE* fichier);
bool check_version_xml(FILE* fichier, xml_t* xml_entree);
bool check_retour_xml(FILE* fichier);
bool check_parent_xml(FILE* fichier, xml_t* xml_entree, elementXml_t* element_entree);
bool check_tab_xml(FILE* fichier);
bool check_fils_xml(FILE* fichier, xml_t* xml_entree, elementXml_t* element_entree);

#endif
