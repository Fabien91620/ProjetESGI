#ifndef __XML_H__
#define __XML_H__

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define ERROR_ALLOCATION {printf("Erreur lors de l'allocation mémoire.\nLe programme va s'arreter.\n");exit(1);}
#define characterStringSize 100
#define skipSpaces skip(filePointer, (int)' ')
#define skipTabs skip(filePointer, (int)'\t')
#define skipLinesBreak skip(filePointer, (int)'\n')
#define skipSpacesTabsLinesBreak skip(filePointer, (int)'*')

//  Caluculateur de la ligne d'érreur en cas d'érreur dans le fichier
#define errorLineFile {if(character==(int)'\n') LineXmlFile++;}
#define errorLineFilePrint {printf("Le format du fichier est incorrect.\nErreur à la ligne : N°%d\n", LineXmlFile);}


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
    int childrenNumber;	         //	Si l'élément est de type SUBELEMENT si toujours 0
    xmlElement_t* subXmlElement; //	Si l'élément est de type SUBELEMENT si non NULL
    xmlElement_t* nextElement;
};

//  XML
struct xml{
    char* version;
    xmlElement_t* firstXmlElement;
    int elementsNumber;
};



// ***************************************************************
// **************************Fonctions****************************

xml_t* checkXml(FILE* filePointer); // Fonction principale
void destroy(xml_t** xml);          // Libération de mémoire
void xmlPrint(xml_t* xml); // Affichage d'un xml


#endif
