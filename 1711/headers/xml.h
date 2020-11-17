#ifndef __XML_H__
#define __XML_H__

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define ERROR_ALLOCATION_XML {printf("Erreur lors de l'allocation mémoire.\nLe programme va s'arreter.\n");exit(1);}
#define characterStringSizeXml 100
#define skipSpacesXml skip(filePointer, (int)' ')
#define skipTabsXml skip(filePointer, (int)'\t')
#define skipLinesBreakXml skip(filePointer, (int)'\n')
#define skipSpacesTabsLinesBreakXml skip(filePointer, (int)'*')

//  Caluculateur de la ligne d'érreur en cas d'érreur dans le fichier
#define errorLineXmlFile {if(character==(int)'\n') LineXmlFile++;}
#define errorLineXmlFilePrint {printf("Erreur dans le fichier XML à la ligne : N°%d\n", LineXmlFile);}


// ***************************************************************
// **************************Structure****************************

typedef struct xmlElement xmlElement_t;
typedef struct xml xml_t;

//  ELEMENT
struct xmlElement{
    char* name;
    enum{
		PCDATA_TYPE,             //	Données textuelles de type (#PCDATA)
		SUBELEMENT_TYPE          //	Un autre élément pourra servir de type
	}elementType;
    char* data;                  //	Si l'élément est de type PCDATA
    int childrenNumber;	         //	Si l'élément est de type SUBELEMENT si non 0
    xmlElement_t* subXmlElement; //	Si l'élément est de type SUBELEMENT si non NULL
    xmlElement_t* nextElement;   // L'élément suivant
};

//  XML
struct xml{
    char* version;
    xmlElement_t* firstXmlElement;
    int elementsNumber;
};



// ***************************************************************
// **************************Fonctions****************************

xml_t* checkXml(FILE* filePointer);     // Fonction principale (vérification et enregistrement dans la structure)
void destroyXml(xml_t** xml);           // Libération de mémoire
void printXml(xml_t* xml);              // Affichage des données de la structure du xml


#endif