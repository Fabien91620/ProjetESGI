#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "xml/xml.h"

int main(){
    FILE* fichierXml = fopen("xml/xml.xml", "r");
    if(!fichierXml){
        printf("%s\n", "Erreur lors de l'ouverture du ficher xml !\nLe programme va s'arreter");
        exit(1);
    }
    xml_t* xml = checkXml(fichierXml);
    if(xml){
        printf("%s\n", "-> XML : Format correct.\nLes données du fichier ont été sauvegardé dans \"(xmlElement_t) xml\".\n");
        xmlPrint(xml);
        destroy(&xml);
    }

    fclose(fichierXml);
	return EXIT_SUCCESS;
}