#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "dtd.h"
#include "xml.h"

int main(){
	FILE* fichier = fopen("/Users/fabienbarrios/Documents/Fabien/ESGI/Bachelor/Project_ESGI/Project_ESGI/dtd.dtd", "r");
	if(!fichier){
		printf("%s\n", "Erreur lors de l'ouverture du ficher !\nLe programme va s'arreter");
		exit(1);
	}
	dtd_t* dtd_entree = malloc(sizeof(dtd_t));
//	printf("--%d--\n",check_type(fichier));
//	printf("--%d--\n",check_one_space(fichier));
//	printf("--%d--\n",check_name(fichier, dtd_entree));
//	printf("%s\n", dtd_entree -> name);
//	printf("%ld\n", strlen(dtd_entree -> name));

    
    FILE* fichierXml = fopen("/Users/fabienbarrios/Documents/Fabien/ESGI/Bachelor/Project_ESGI/Project_ESGI/xml.xml", "r");
    if(!fichierXml){
        printf("%s\n", "Erreur lors de l'ouverture du ficher xml !\nLe programme va s'arreter");
        exit(1);
    }
    xml_t* xml_entree = malloc(sizeof(xml_t));
    elementXml_t* element_entree = malloc(sizeof(elementXml_t));
    
    if(check_xml(fichierXml, xml_entree, element_entree)){
        printf("%s\n", "XML Bon");
    }
    else{
        printf("%s\n", "XML Mauvais");
    }
    
    
    
    
    
//    free(dtd_entree -> name);
    free(dtd_entree);
    free(xml_entree);
    free(xml_entree ->premier_fils);
//    free(element_entree);
    free(element_entree->name);
    fclose(fichier);
    fclose(fichierXml);
	return 0;
}
