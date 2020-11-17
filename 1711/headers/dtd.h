#ifndef __DTD_H__
#define __DTD_H__

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define ERROR_ALLOCATION_DTD {printf("Erreur lors de l'allocation mémoire.\nLe programme va s'arreter.\n");exit(1);}
#define characterStringSizeDtd 100
#define skipSpacesDtd skipInDtdFile(filePointer, (int)' ')
#define skipTabsDtd skipInDtdFile(filePointer, (int)'\t')
#define skipLinesBreakDtd skipInDtdFile(filePointer, (int)'\n')
#define skipSpacesTabsLinesBreakDtd skipInDtdFile(filePointer, (int)'*')

//  Caluculateur de la ligne d'érreur en cas d'érreur dans le fichier
#define errorLineDtdFile {if(character==(int)'\n') LineDtdFile++;}
#define errorLineDtdFilePrint {printf("Erreur dans le fichier DTD à la ligne : N°%d\n", LineDtdFile);}


// ***************************************************************
// **************************Structure****************************

typedef struct dtdElement dtdElement_t;
typedef struct dtd dtd_t;
		
//  ELEMENT
struct dtdElement{
	char* name;
	enum{
		PCDATA_TYPE_DTD,			//	Données textuelles de type (#PCDATA)
		SUBELEMENT_TYPE_DTD,		//	Un autre élément pourra servir de type
		EMPTY_TYPE					//	Vide
	}dtdElementType;
	//	Si de type SUBELEMENT_TYPE
	struct{
		char* parameter;		// Le parametre entre parenthèses
		enum{
			ONLY_ONE,			// rien
			ONE_OR_MORE,		// +
			ZERO_OR_MORE,		// *
			ZERO_OR_ONE			// ?
		}dtdElementParameterNumber;	
	}dtdElementParameter;
	dtdElement_t* subDtdElement;	// Le pointeur vers l'élément en parametre
	dtdElement_t* nextDtdElement;   // L'élément suivant
};

//	DTD
struct dtd{
	char* root;
	int dtdElementsNumber;
	dtdElement_t* firstDtdElement;
};



// ***************************************************************
// **************************Fonctions****************************

dtd_t* checkDtd(FILE* filePointer);
void destroyDtd(dtd_t** dtd);
void destroyDtdElements(dtdElement_t* e);
void dtdPrint(dtd_t* dtd);



#endif
