#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "xml.h"


int main(){
	
    FILE* fichierXml = fopen("xml.xml", "r");
    if(!fichierXml){
        printf("%s\n", "Erreur lors de l'ouverture du ficher xml !\nLe programme va s'arreter");
        exit(1);
    }
    xml_t* xml = malloc(sizeof(xml_t));

    if(check_xml(fichierXml, xml)){
        printf("%s\n", "XML Bon");
    }
    else{
        printf("%s\n", "XML Mauvais");
    }
    afficheXML(xml);

    destroy(xml);
    
    fclose(fichierXml);


	return EXIT_SUCCESS;
}
