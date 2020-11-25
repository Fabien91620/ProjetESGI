/* 
* @Author HS <hsaghir@myges.fr>
* @Developed on Nov 2020
* @Description : program using the xml and dtd libraries 
to check the conformity of an xml file with a dtd file
*/
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "../headers/xml.h"
#include "../headers/dtd.h"
#include "../headers/menu.h"

#define filesOk {printf("Succès : Les fichiers sont conformes.");}
#define filesError {printf("Erreur : Les fichiers ne sont pas conformes.");}

bool ON = true;             //  VARIABLE POUR ARRETER LE PROGRAMME
xml_t* xml = (xml_t*)NULL;  //  SAUVEGARDE DES DONNEES EXTRAITE DU FICHIER XML
dtd_t* dtd = (dtd_t*)NULL;	//  SAUVEGARDE DES DONNEES EXTRAITE DU FICHIER DTD
char xmlFileName[100]={0};  //  SAUVEGARDE DU NOM DU FICHIER XML PASSEE EN PARAMTERE AU LANCEMENT
char dtdFileName[100]={0};  //  SAUVEGARDE DU NOM DU FICHIER DTD PASSEE EN PARAMTERE AU LANCEMENT

menu_t* m = (menu_t*)NULL;  //  Menu

/* FONCTION D'APPEL DE LA FONCTION PRINCIPALE DE LA LIBRAIRIE DTD, DE CHARGEMENT DES DONNEES 
DTD DEPUIS UN FICHIER DTD ET SAUVEGARDE */
void xmlLoad() {
    system("clear");
    if(xml){
        printf("INFORMATION : Les anciennes données XML sauvegardées ont été ecrasées.\n\n");
        setItemEnabled(m, 4, false);
        setItemEnabled(m, 3, false);
        destroyXml(&xml);
        xml = (xml_t*)NULL;
    }
    FILE* xmlFilePointer = (FILE*)NULL;
    char string[100] = {0};
    if(!xmlFileName[0]){
        printf("<= Veuillez entrer le nom du fichier XML à charger: \n>> (Le fichier doit se trouver dans le répértoire de cette application)\n\n");
        scanf("%s", string);
        system("clear");
    }else{
        strcpy(string, xmlFileName);
    }
    xmlFilePointer = fopen(string, "r");
    if(!xmlFilePointer){
        printf("ERREUR : Erreur lors de l'ouverture du ficher \"%s\".\n>> (Vérifier le nom et le chemin de votre fichier)\n\n", string);
        xmlFileName[0]=0;
        return;
    }
    xml = checkXml(xmlFilePointer);
    if(xml){
        printf(">> XML : Format correct.\n>> Les données du fichier \"%s\" ont été sauvegardées.\n\n", string);
        setItemEnabled(m, 4, true);
        if(dtd) setItemEnabled(m, 3, true);
        
    }else{
        printf(">> XML : Format incorrect.\n>> Les données du fichier \"%s\" n'ont PAS été sauvegardées.\n\n", string);
    }
    xmlFileName[0]=0;
    fclose(xmlFilePointer);
}
 
/* FONCTION D'APPEL DE LA FONCTION PRINCIPALE DE LA LIBRAIRIE XML, DE CHARGEMENT DES DONNEES 
XML DEPUIS UN FICHIER XML ET SAUVEGARDE */
void dtdLoad() {
    if(!dtdFileName[0])system("clear"); // POUR NE PAS EFFACER LA PREMIER FOIS SI JE CHARGE UN XML
    if(dtd){
        printf("INFORMATION : Les anciennes données DTD sauvegardées ont été ecrasées.\n\n");
        setItemEnabled(m, 5, false);
        setItemEnabled(m, 3, false);
        destroyDtd(&dtd);
        dtd = (dtd_t*)NULL;
    }
    FILE* dtdFilePointer = (FILE*)NULL;
    char string[100] = {0};
    if(!dtdFileName[0]){
        printf("<= Veuillez entrer le nom du fichier DTD à charger: \n>> (Le fichier doit se trouver dans le répértoire de cette application)\n\n");
        scanf("%s", string);
        system("clear");
    }else{
        strcpy(string, dtdFileName);
    }
    dtdFilePointer = fopen(string, "r");
    if(!dtdFilePointer){
        printf("ERREUR : Erreur lors de l'ouverture du ficher \"%s\".\n>> (Vérifier le nom et le chemin de votre fichier)\n\n", string);
        dtdFileName[0] = 0;
        return;
    }
    dtd = checkDtd(dtdFilePointer);
    if(dtd){
        printf(">> DTD : Format correct.\n>> Les données du fichier \"%s\" ont été sauvegardées.\n\n", string);
        setItemEnabled(m, 5, true);
        if(xml) setItemEnabled(m, 3, true);
    }
    else{
        printf(">> DTD : Format incorrect.\n>> Les données du fichier \"%s\" n'ont PAS été sauvegardées.\n\n", string);
    }
    dtdFileName[0] = 0;
    fclose(dtdFilePointer);
}

/* FONCTION DE PARCOURS DU DTD ET RECHERCHE D'ELEMENT ASSOCIE A L'ELEMENT XML
PASSE EN PARAMETRE */
dtdElement_t* findTheAssociateDtdElement(xmlElement_t* xmlElement, dtdElement_t* dtdElement){
	if(!xmlElement || !dtdElement) return NULL;
	while(dtdElement){
		if(!(dtdElement -> checked) && !(strcmp(dtdElement -> name, xmlElement -> name))){
			if((int)(dtdElement -> dtdElementType) == (int)(xmlElement -> xmlElementType)) return dtdElement;
			else return NULL;
		}
		dtdElement = dtdElement -> nextDtdElement;
	}
	return NULL;
}

/* FONCTION D'AFFICHAGE DU NOM ET DU NUMERO DE LA LIGNE DE L'ELEMENT
CONTENANT UNE ERREUR DE CONFORMITE DE NOM */
bool attributError(char* name, int lineNumber){
    printf(">> Erreur : L'attribut de l'élément XML \"%s\" (ligne n°%d) n'a pas d'équivalent dans le fichier DTD.\n", name, lineNumber);
    return false;
}

bool nameAndTypeError(char* name, int lineNumber){
    printf(">> Erreur : L'élément XML \"%s\" (ligne n°%d) n'a pas d'équivalent dans le fichier DTD.\n", name, lineNumber);
    return false;
}

/* FONCTION D'AFFICHAGE DU NOM ET DU NUMERO DE LA LIGNE DE L'ELEMENT
CONTENANT UNE ERREUR DE CONFORMITE DE NOMBRE DE SOUS-ELEMENT */
bool elementsNumberError(char* name, int lineNumber){
    printf(">> Erreur : Le nombre des sous-éléments XML de l'élément \"%s\" (ligne n°%d) ne correspond pas à sa définition dans le fichier DTD.\n", name, lineNumber);
    return false;
}

// FONCTION DE VERIFICATION SI UN ELEMENT DTD A DEJA UN ELEMENT XML ASSOCIE
bool checkIfDtdElementsAreAllCheked(dtdElement_t* dtdElement){
    while(dtdElement){
        if(!(dtdElement -> checked)){
            printf(">> Erreur : L'élément DTD \"%s\" (ligne n°%d) n'a pas d'équivalent dans le fichier XML.\n", dtdElement -> name, dtdElement -> errorLine);
            return false;
        }
        dtdElement -> checked = false;
        dtdElement_t* subDtdElement = dtdElement -> subDtdElement;
        while(subDtdElement){
            if(!(subDtdElement -> checked)){
                printf(">> Erreur : L'élément DTD \"%s\" (ligne n°%d) n'a pas d'équivalent dans le fichier XML.\n", dtdElement -> name, dtdElement -> errorLine);
                return false;
            }
            subDtdElement -> checked = false;
            subDtdElement = subDtdElement -> nextDtdElement;
        }
        dtdElement = dtdElement -> nextDtdElement;
    }
    return true;
}

/* FONCTION DE COMPARAISON DES ELEMENT XML AUX ELEMENTS DTD, APPELEES
PAR LA FONCTION PRICIPALE DE VERIFICATION DE LA CONFORMITE DES DONNEES
XML SAUVEGARDEES AVEC LES DONNEES DTD SAUVEGARDEES */
bool compareXmlElementsToDtdElements(xmlElement_t* xmlElement, dtdElement_t* dtdElement){
	while(xmlElement){
		dtdElement_t* associateDtdElement = findTheAssociateDtdElement(xmlElement, dtdElement);
		xmlElement_t* itteratorXmlSubElement = (xmlElement_t*)NULL;									
        if(!associateDtdElement){
            printf(">> Erreur : L'élément XML \"%s\" (ligne n°%d) n'a pas d'équivalent dans le fichier DTD.\n", xmlElement -> name, xmlElement -> errorLine);
            return false;
        }
        associateDtdElement -> checked = true;
        if(xmlElement -> attlist){
            if(associateDtdElement -> attlist){ 
                if(strcmp(associateDtdElement -> attlist -> attributName, xmlElement -> attlist -> attributName)) attributError(xmlElement -> name,  xmlElement -> errorLine);
            }else attributError(xmlElement -> name,  xmlElement -> errorLine);      
        }else{
            if(associateDtdElement -> attlist) attributError(xmlElement -> name,  xmlElement -> errorLine);
        }
        if(associateDtdElement -> dtdElementType == SUBELEMENT_TYPE_DTD){
			switch(associateDtdElement -> dtdElementParameter.dtdElementParameterNumber){
				case ONLY_ONE:		if((xmlElement -> childrenXmlElementNumber) != 1) return elementsNumberError(xmlElement -> name, xmlElement -> errorLine);
                                    itteratorXmlSubElement = xmlElement -> subXmlElement;
                                    if(!associateDtdElement -> subDtdElement) return nameAndTypeError(itteratorXmlSubElement -> name,  itteratorXmlSubElement -> errorLine);
									if(strcmp(itteratorXmlSubElement -> name, associateDtdElement -> subDtdElement -> name)) return nameAndTypeError(itteratorXmlSubElement -> name,  itteratorXmlSubElement -> errorLine);
									if((int)(itteratorXmlSubElement -> xmlElementType) != (int)(associateDtdElement -> subDtdElement -> dtdElementType)) return nameAndTypeError(itteratorXmlSubElement -> name,  itteratorXmlSubElement -> errorLine);
									associateDtdElement -> subDtdElement -> checked = true;
									break;
				case ONE_OR_MORE:	if((xmlElement -> childrenXmlElementNumber) < 1) return elementsNumberError(xmlElement -> name, xmlElement -> errorLine);
                                    itteratorXmlSubElement = xmlElement -> subXmlElement;
									if(!associateDtdElement -> subDtdElement) return nameAndTypeError(itteratorXmlSubElement -> name,  itteratorXmlSubElement -> errorLine);
									while(itteratorXmlSubElement){
										if(strcmp(itteratorXmlSubElement -> name, associateDtdElement -> subDtdElement -> name)) return nameAndTypeError(itteratorXmlSubElement -> name,  itteratorXmlSubElement -> errorLine);
										if((int)(itteratorXmlSubElement -> xmlElementType) != (int)(associateDtdElement -> subDtdElement -> dtdElementType)) return nameAndTypeError(itteratorXmlSubElement -> name,  itteratorXmlSubElement -> errorLine);										
                                        itteratorXmlSubElement = itteratorXmlSubElement -> nextXmlElement;
									}		
									associateDtdElement -> subDtdElement -> checked = true;
									break;
				case ZERO_OR_MORE:	if((xmlElement -> childrenXmlElementNumber) >= 1){
                                        if(!associateDtdElement -> subDtdElement) return elementsNumberError(xmlElement -> name, xmlElement -> errorLine);
                      					itteratorXmlSubElement = xmlElement -> subXmlElement;
										while(itteratorXmlSubElement){
											if(strcmp(itteratorXmlSubElement -> name, associateDtdElement -> subDtdElement -> name)) return nameAndTypeError(itteratorXmlSubElement -> name,  itteratorXmlSubElement -> errorLine);
											if((int)(itteratorXmlSubElement -> xmlElementType) != (int)(associateDtdElement -> subDtdElement -> dtdElementType)) return nameAndTypeError(itteratorXmlSubElement -> name,  itteratorXmlSubElement -> errorLine);
											itteratorXmlSubElement = itteratorXmlSubElement -> nextXmlElement;
										}		
										associateDtdElement -> subDtdElement -> checked = true;
									}
									break;

				case ZERO_OR_ONE:	if((xmlElement -> childrenXmlElementNumber) > 1) return elementsNumberError(xmlElement -> name, xmlElement -> errorLine);
									if((xmlElement -> childrenXmlElementNumber) == 1){
                                        if(!associateDtdElement -> subDtdElement) return nameAndTypeError(itteratorXmlSubElement -> name,  itteratorXmlSubElement -> errorLine);
										itteratorXmlSubElement = xmlElement -> subXmlElement;
										if(strcmp(itteratorXmlSubElement -> name, associateDtdElement -> subDtdElement -> name)) return nameAndTypeError(itteratorXmlSubElement -> name,  itteratorXmlSubElement -> errorLine);
										if((int)(itteratorXmlSubElement -> xmlElementType) != (int)(associateDtdElement -> subDtdElement -> dtdElementType)) return nameAndTypeError(itteratorXmlSubElement -> name,  itteratorXmlSubElement -> errorLine);
										associateDtdElement -> subDtdElement -> checked = true;
									}
									break;
			}
		}
		xmlElement = xmlElement -> nextXmlElement;
	}
    return checkIfDtdElementsAreAllCheked(dtdElement);
}

/* FONCTION PRINCIPALE DE VERIFICATION DE LA CONFORMITE DES 
DONNEES XML SAUVEGARDEES AVEC LES DONNEES DTD SAUVEGARDEES */
void compareXmlToDtd(){
    system("clear");
    if(!xml) printf(">> Pas de fichier XML en mémoire, veuillez charger un fichier.\n\n");
    if(!dtd) printf(">> Pas de fichier DTD en mémoire, veuillez charger un fichier.\n\n");
    if(!xml && !dtd) return;
    if(!dtd -> dtdElementsNumber && !xml -> xmlElementsNumber){ // Pas d'éléments dans DTD et pas d'éléments dans XML
    	filesOk;
    	return;
    }
    if(!dtd -> dtdElementsNumber || !xml -> xmlElementsNumber){
    	filesError;
    	return;
	}
	bool response = compareXmlElementsToDtdElements(xml -> firstXmlElement, dtd -> firstDtdElement);
    if(response){
        printf(">> XML-DTD : Le DTD est conforme avec le fichier XML.\n\n");
   
    }else{
        printf(">> XML-DTD : Erreur, Le DTD n'est conforme avec le fichier XML.\n\n");   
    }
}

// AFFICHAGE DU XML SAUVEGARDE
void xmlPrintData() {
    system("clear");
    printXml(xml);
    printf("\n\n");
}
 
// AFFICHAGE DU DTD SAUVEGARDE
void dtdPrintData() {
    system("clear");
    dtdPrint(dtd);
    printf("\n\n");
}

// QUITTER LE PROGRAMME
void quit() {
    ON = false;
}

// FONCTION DE TRAITEMENT D'ARGUMENTS(PARAMETRES) ENTRES AU LANCEMENT
void checkArgs(int argc, char** argv){
    if(argc >= 2){
        if(strlen(argv[1])>=5 && argv[1][strlen(argv[1])-1]=='l' && argv[1][strlen(argv[1])-2]=='m' && argv[1][strlen(argv[1])-3]=='x' && argv[1][strlen(argv[1])-4]=='.'){
            strcpy(xmlFileName, argv[1]);
            xmlLoad();
        }
    }
    if(argc >= 3){
        if(strlen(argv[2])>=5 && argv[2][strlen(argv[2])-1]=='d' && argv[2][strlen(argv[2])-2]=='t' && argv[2][strlen(argv[2])-3]=='d' && argv[2][strlen(argv[2])-4]=='.'){
            strcpy(dtdFileName, argv[2]);
            dtdLoad();
        }
    }
}


int main(int argc, char** argv) {
    system("clear");    
    m = createMenu("Main menu");
    addMenuAction(m,"Charger et vérifier un fichier XML",xmlLoad, true);
    addMenuAction(m,"Charger et vérifier un fichier DTD",dtdLoad, true);
    addMenuAction(m,"Vérifier la conformité DTD-XML",compareXmlToDtd, false);
    addMenuAction(m,"Afficher les données XML sauvegardées",xmlPrintData, false);
    addMenuAction(m,"Afficher les données DTD sauvegardées",dtdPrintData, false);
    addMenuAction(m,"Quitter",quit, true);
    checkArgs(argc, argv);
    while(ON) launchMenu(m);
    deleteMenu(m);
    if(xml) destroyXml(&xml);
    if(dtd) destroyDtd(&dtd);
    return 0;
}