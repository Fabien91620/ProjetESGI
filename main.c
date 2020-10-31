#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "dtd.h"
#include "xml.h"

int main(){
	
    FILE* fichierXml = fopen("/Users/fabienbarrios/Documents/Fabien/ESGI/Bachelor/Project_ESGI/Project_ESGI/xml.xml", "r");
    if(!fichierXml){
        printf("%s\n", "Erreur lors de l'ouverture du ficher xml !\nLe programme va s'arreter");
        exit(1);
    }
    xml_t* xml_entree = malloc(sizeof(xml_t));
    
    if(check_xml(fichierXml, xml_entree)){
        printf("%s\n", "XML Bon");
    }
    else{
        printf("%s\n", "XML Mauvais");
    }
    
    printf("%s\n", xml_entree->premier_fils->name);
    
    fclose(fichierXml);
	return 0;
}
