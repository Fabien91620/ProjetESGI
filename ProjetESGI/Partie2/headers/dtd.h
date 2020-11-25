/* 
* @Author HS <hsaghir@myges.fr>
* @Developed on Nov 2020
* @Description : presentation of the structure and the functions of the library of checking
the format of a dtd file and extracting its data
*/
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
#define skipSpacesTabsDtd skipInDtdFile(filePointer, (int)'+')
#define skipSpacesTabsLinesBreakDtd skipInDtdFile(filePointer, (int)'*')

// COMPTEUR DE LA LIGNE D'ERREUR EN CAS D'ERREUR DANS LE FICHIER
#define errorLineDtdFile {if(character==(int)'\n') LineDtdFile++;}
// AFFICHAGE DE LA LIGNE D'ERREUR EN CAS D'ERREUR DANS LE FICHIER
#define errorLineDtdFilePrint {printf("Erreur dans le fichier DTD à la ligne : N°%d\n", LineDtdFile);}


// ***************************************************************
// **************************STRUCTURE****************************

typedef struct dtdAttlist dtdAttlist_t;
typedef struct dtdElement dtdElement_t;
typedef struct dtd dtd_t;
		

struct dtdAttlist{
	char* elementName;				//	NOM DE L'ELEMENT ASSOCIE
	char* attributName;				//	NOM DE L'ATTRIBUT
	char* defaultValue; 			// 	VALEUR PAR DEFAULT, PEUT ETRE NULL
};


//  ELEMENT
struct dtdElement{
	char* name;
	dtdAttlist_t* attlist;			// 	POINTEUR VERS L'ATTRIBUT ASSOCIE A L'ELEMENT, PAR DEFAULT NULL
	enum{
		PCDATA_TYPE_DTD,			//	TYPE PCDATA
		SUBELEMENT_TYPE_DTD,		//	TYPE SOUS-ELEMENT
	}dtdElementType;
									//	SI DE TYPE SOUS-ELEMENT
	struct{
		char* parameter;			//	LE NOM DU SOUS FILS ENTRE PARENTHESES
		enum{
			ONLY_ONE,				//	SANS
			ONE_OR_MORE,			//	+
			ZERO_OR_MORE,			//	*
			ZERO_OR_ONE				//	?
		}dtdElementParameterNumber;	//	NOMBRE MINIMUM ET MAXIMUM DE SOUS-ELEMENT
	}dtdElementParameter;
	dtdElement_t* subDtdElement;	//	PREMIER SOUS-ELEMENT
	dtdElement_t* nextDtdElement;   //	ELEMENT SUIVANT
	
	bool checked;					//	UTILISEE POUR LA VERIFICATION DANS LE MAIN
	int errorLine;					//	NUMERO DE LA LIGNE DE L'ELEMENT DANS FICHIER
};

//	DTD
struct dtd{
	char* root;						//	RACINE(NOM DU FICHIER)
	int dtdElementsNumber;			//	NOMBRE D'ELEMENTS
	dtdElement_t* firstDtdElement;	//	PREMIER ELEMENT
};



// ***************************************************************
// *****************PRINCIPALES FONCTIONS*************************

/*
*/
dtd_t* checkDtd(FILE* filePointer);


/*
LA FONCTION PREND EN PARAMETRE UN POINTEUR VERS UN POINTEUR VERS UNE STRUCTURE DE DONNEE DE
TYPE DTD_T, PARCOURT LA STRUCTURE ET LIBERE TOUTE LA MEMOIRE OCCUPEE PAR SES ELEMENTS ET LEURS
SOUS-ELEMENT ET ENFIN LE DTD LUI MEME, LE PONTEUR VERS LA STRUCTURE EST MIT A NULL A LA FIN.
*/
void destroyDtd(dtd_t** dtd);

/*
LA FONCTION PREND EN PARAMETRE UN POINTEUR VERS UNE STRUCTURE DE DONNEE DE
TYPE ELEMENT_T, PARCOURT LA STRUCTURE ET LIBERE TOUTE LA MEMOIRE OCCUPEE PAR SES SOUS-ELEMENTS
ET LES ELEMENTS SUIVANTS.
*/
void destroyDtdElements(dtdElement_t* e);

/*
LA FONCTION PREND EN PARAMETRE UN POINTEUR VERS UNE STRUCTURE DE DONNEE DE
TYPE DTD_T, PARCOURT LA STRUCTURE ET AFFICHE SES ELEMENTS ET LEURS SOUS-ELEMENTS.
*/
void dtdPrint(dtd_t* dtd);



#endif
