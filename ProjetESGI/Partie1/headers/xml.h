/* 
* @Author HS <hsaghir@myges.fr>
* @Developed on Nov 2020
* @Description : presentation of the structure and the functions of the library of checking
the format of an xml file and extracting its data
*/
#ifndef __XML_H__
#define __XML_H__

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define ERROR_ALLOCATION_XML {printf("Erreur lors de l'allocation mémoire.\nLe programme va s'arreter.\n");exit(1);}
#define characterStringSizeXml 100
#define skipSpacesXml skipInXmlFile(filePointer, (int)' ')
#define skipTabsXml skipInXmlFile(filePointer, (int)'\t')
#define skipLinesBreakXml skipInXmlFile(filePointer, (int)'\n')
#define skipSpacesTabsLinesBreakXml skipInXmlFile(filePointer, (int)'*')

// COMPTEUR DE LA LIGNE D'ERREUR EN CAS D'ERREUR DANS LE FICHIER
#define errorLineXmlFile {if(character==(int)'\n') LineXmlFile++;}
// AFFICHAGE DE LA LIGNE D'ERREUR EN CAS D'ERREUR DANS LE FICHIER
#define errorLineXmlFilePrint {printf("Erreur dans le fichier XML à la ligne : N°%d\n", LineXmlFile);}


// ***************************************************************
// **************************STRUCTURE****************************

typedef struct xmlElement xmlElement_t;
typedef struct xml xml_t;

//  ELEMENT
struct xmlElement{
    char* name;
    enum{
		PCDATA_TYPE_XML,              //   TYPE PCDATA
		SUBELEMENT_TYPE_XML,          //   TYPE SOUS-ELEMENT
    }xmlElementType;
    char* data;                       //   SI DE TYPE PCDATA
    int childrenXmlElementNumber;	  //   SI DE TYPE SOUS-ELEMENT
    xmlElement_t* subXmlElement;      //   PREMIER SOUS-ELEMENT
    xmlElement_t* nextXmlElement;     //   L'ELEMENT SUIVANT
    int errorLine;
};

//  XML
struct xml{
    char* version;                    //    VERSION DU FICHIER
    int xmlElementsNumber;            //    NOMBRE D'ELEMENTS
    xmlElement_t* firstXmlElement;    //    PREMIER ELEMENT
};



// ***************************************************************
// ******************PRINCIPALES FONCTIONS************************

/*
LA FONCTION PREND UN PONTEUR D'UN FICHIER XML OUVERT EN LECTURE, VERIFIE SON FORMAT ET
RENVOIE UN POINTEUR VERS UNE STRUCTURE DE DONNEE DE TYPE XML_T CONTENANT LES DONNEES EXTRAITES
DU FICHIER OU NULL EN CAS D'ERREUR (DE FORMAT OU D'ALLOCATION).
EN CAS D'ERREUR DE FORMAT LA VARIABLE errorLineXmlFile CONTIENT LA LIGNE DE L'ERREUR, LE 
PROGRAMME EST ALORS ARRETE ET TOUTE LA MEMOIRE LIBEREE.
EN CAS D'ERREUR D'ALLOCATION, UNE ERREUR EST AFFICHEE EGALEMENT ET LA MEMOIRE LIBREE ET LE
PROGRAMME S'ARRETE.
*/
xml_t* checkXml(FILE* filePointer);

/*
LA FONCTION PREND EN PARAMETRE UN POINTEUR VERS UN POINTEUR VERS UNE STRUCTURE DE DONNEE DE
TYPE XML_t, PARCOURT LA STRUCTURE ET LIBERE TOUTE LA MEMOIRE OCCUPEE PAR SES ELEMENTS ET LEURS
SOUS-ELEMENT ET ENFIN LE XML LUI MEME, LE PONTEUR VERS LA STRUCTURE EST MIT A NULL A LA FIN.
*/
void destroyXml(xml_t** xml);

/*
LA FONCTION PREND EN PARAMETRE UN POINTEUR VERS UNE STRUCTURE DE DONNEE DE
TYPE XML_T, PARCOURT LA STRUCTURE ET AFFICHE SES ELEMENTS ET LEURS SOUS-ELEMENTS.
*/
void printXml(xml_t* xml);


#endif