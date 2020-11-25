/* 
* @Author HS <hsaghir@myges.fr>
* @Developed on Nov 2020
* @Description : library of checking the format of a dtd file and extracting its data
*/
#include "../headers/dtd.h"

// COMPTEUR DE LA LIGNE D'ERREUR EN CAS D'ERREUR DANS LE FICHIER
int LineDtdFile = 1;

// ***************************************************************
// ****FONCTIONS DE LIBERATION DE MEMOIRE (Garbage Collector)****

// LIBERER UN ATTLIST
void destroyAttlist(dtdAttlist_t* attlist){
	free(attlist -> elementName);
	free(attlist -> attributName);
	if(attlist -> defaultValue) free(attlist -> defaultValue);
	free(attlist); 
}

// LIBERER UN ELEMENT DTD, SES SOU-ELEMENTS ET LES ELEMENTS SUIVANTS
void destroyDtdElements(dtdElement_t* e){
    if(!e) return;
    if(e -> dtdElementType == SUBELEMENT_TYPE_DTD){
        free(e -> dtdElementParameter.parameter);
        // A revérifier
        destroyDtdElements(e -> subDtdElement);
        e -> subDtdElement = NULL;   
    }
    destroyDtdElements(e-> nextDtdElement);
    free(e -> name);
    free(e);
}

// LIBERER UN DTD DE TYPE DTD_T
void destroyDtd(dtd_t** dtd){
    if(!(*dtd)) return;
    destroyDtdElements((*dtd) -> firstDtdElement);
    free((*dtd) -> root);
    free(*dtd);
    (*dtd) = (dtd_t*)NULL;
}



// ***************************************************************
// ****FONTIONS DE VERIFICATION ET DE RECUPERATION DE DONNEES****

// FONCTION QUI IGNORE LES CHARACTERES D'ESPACEMENT DANS LE FICHIER DTD
void skipInDtdFile(FILE* filePointer, int toSkip){
    if(!filePointer) return;
    int character = fgetc(filePointer);
    if(toSkip == (int)'*'){
        while(character == (int)' ' || character == (int)'\n' || character == (int)'\t'){
            errorLineDtdFile;
            character = fgetc(filePointer);
        }
    }else if(toSkip == (int)'+'){
        while(character == (int)' '|| character == (int)'\t'){
            errorLineDtdFile;
            character = fgetc(filePointer);
        }
    }else{
        while(character == toSkip){
            errorLineDtdFile;
            character = fgetc(filePointer);
        }
    }
    if(character != EOF)fseek(filePointer, -1, SEEK_CUR);
}

// AJOUTER UN ELEMENT SUIVANT A UN ELEMENT DTD
void addNextdtdElementToDtdElement(dtdElement_t* e, dtdElement_t* elementToAdd){
    if(!e || !elementToAdd) return;
    dtdElement_t* itterator = e -> nextDtdElement;
    if(!itterator){
        e -> nextDtdElement = elementToAdd;
        return;
    }
    while(itterator -> nextDtdElement){
        itterator = itterator -> nextDtdElement;
    }
    itterator -> nextDtdElement = elementToAdd;
    elementToAdd -> nextDtdElement = (dtdElement_t*)NULL;
}

// PARCOURIR LE DTD ET AJOUTER LES SOUS-ELEMENT DTD A L'ELEMENT DTD PARENT ASSOCIES
bool serachForChildren(dtd_t* dtd, int* children){
	if(!dtd || !dtd -> firstDtdElement || !dtd -> dtdElementsNumber) return false;
    dtdElement_t* saveElement = (dtdElement_t*)NULL;
    dtdElement_t* itteratorDtdElements = (dtdElement_t*)NULL;
    for(int i = 1; i<=dtd -> dtdElementsNumber; i++){
        saveElement = dtd -> firstDtdElement;
        dtd -> firstDtdElement = dtd -> firstDtdElement -> nextDtdElement;
        saveElement -> nextDtdElement = (dtdElement_t*)NULL;
        itteratorDtdElements = dtd -> firstDtdElement;
        bool added = false;
        for(int j = dtd -> dtdElementsNumber; itteratorDtdElements && j>0; j--){
            if(!added && itteratorDtdElements -> dtdElementType == SUBELEMENT_TYPE_DTD && !strcmp(itteratorDtdElements -> dtdElementParameter.parameter, saveElement -> name)){
                if(itteratorDtdElements -> subDtdElement){
                    if(!added) addNextdtdElementToDtdElement(dtd -> firstDtdElement, saveElement);
                    LineDtdFile = saveElement -> errorLine;
                    return false;
                }
                itteratorDtdElements -> subDtdElement = saveElement;
                saveElement -> nextDtdElement = NULL;
                (*children)++;
                added = true;
            }else{
                if(!added && !itteratorDtdElements -> nextDtdElement){
                    if(!dtd -> firstDtdElement) dtd -> firstDtdElement = saveElement;
                    else{
                        addNextdtdElementToDtdElement(dtd -> firstDtdElement, saveElement);
                    }
                }
            }
            itteratorDtdElements = itteratorDtdElements -> nextDtdElement;
        }
        if(!dtd -> firstDtdElement) dtd -> firstDtdElement = saveElement;
    }
    return true;
}

// VERIFIER ET RECUPERER LE PARAMETRE D'UN ELEMENT DTD
bool checkDtdElementParametre(FILE* filePointer, dtdElement_t* e, bool* FormatError){
    if(!filePointer || !e) return false;
    int character = fgetc(filePointer);
    skipSpacesTabsLinesBreakDtd;
    if(character != (int)'(') return false;
    char string[characterStringSizeDtd] = {0};
    int size = 0;
    character = fgetc(filePointer);
    while(character != EOF && ((character >= (int)'a' && character <= (int)'z') || (character >= (int)'A' && character <= (int)'Z')|| (character >= (int)'0' && character <= (int)'9') || character == (int)'-'|| character == (int)'_' || character == (int)'.' || character == (int)'#')){
        if(size && character==(int)'#'){
            *FormatError = true;
            return false;
        }
        string[size] = (char)character;
        size++;
        character = fgetc(filePointer); 
    }
    if(!strcmp(string, e -> name) || (character !=(int)')' && character !=(int)'+' && character !=(int)'*' && character !=(int)'?' && character !=(int)' ' && character !=(int)'\n' && character !=(int)'\t')){
        *FormatError = true;
        return false;
    }
    int specialCharacter = 0;
    if(character !=(int)')'){
        if(character ==(int)'+') specialCharacter = 1;
        else if (character ==(int)'*') specialCharacter = 2;
        else if (character == (int)'?') specialCharacter = 3;
        skipSpacesTabsLinesBreakDtd;
        if(fgetc(filePointer)!=(int)')'){
            *FormatError = true;
            return false;
        }
        skipSpacesTabsLinesBreakDtd;
    }else specialCharacter = 0;
    skipSpacesTabsLinesBreakDtd;
    if(!size){
        if(specialCharacter!=0){
            *FormatError = true;
            return false;
        }
    }else if(!strcmp(string, "#PCDATA")){
        e -> dtdElementType = PCDATA_TYPE_DTD;
        if(specialCharacter != 0)*FormatError = true;
        return (specialCharacter == 0);
    }else{
        if(string[0]==(int)'#'){
            *FormatError = true;
            return false;
        }
        e -> dtdElementType = SUBELEMENT_TYPE_DTD;
        if(specialCharacter == 1) e -> dtdElementParameter.dtdElementParameterNumber = ONE_OR_MORE;
        else if(specialCharacter == 2) e -> dtdElementParameter.dtdElementParameterNumber = ZERO_OR_MORE;
        else if(specialCharacter == 3) e -> dtdElementParameter.dtdElementParameterNumber = ZERO_OR_ONE;
        else if(specialCharacter == 0) e -> dtdElementParameter.dtdElementParameterNumber = ONLY_ONE;
        e -> dtdElementParameter.parameter = malloc(sizeof(char) * (size+1));
        if(!e -> dtdElementParameter.parameter) return false;
        strcpy(e -> dtdElementParameter.parameter, string);
    }
    skipSpacesTabsLinesBreakDtd;
    return true;
}

// VERIFIER ET RECUPERER LE NOM D'UN ELEMENT DTD
bool checkDtdElementAndAttlistNames(bool space, FILE* filePointer, char** name, bool* FormatError){
	if(!filePointer) return false;
	char string[characterStringSizeDtd] = {0};
	int size = 0;
	int character = fgetc(filePointer);
	while(character != EOF && ((character >= (int)'a' && character <= (int)'z') || (character >= (int)'A' && character <= (int)'Z')|| (character >= (int)'0' && character <= (int)'9') || character == (int)'-'|| character == (int)'_' || character == (int)'.')){
  		errorLineDtdFile;
  		string[size] = (char)character;
		size++;
		character = fgetc(filePointer);
	}
	errorLineDtdFile;
	if(!size){
		*FormatError = true;
		return false;
	}
	if(space && character != (int)' ' && character != (int)'\n' && character != (int)'\t'){
		*FormatError = true;
		return false;
	}
	if(!space) fseek(filePointer, -1, SEEK_CUR);
	*name = (char*)malloc(sizeof(char)*(size+1));
	if(!(*name)) return false;
	strcpy(*name, string);
    return true;
}

// VERIFIER LE DEBUT (TYPE ELEMENT) D'UN ELEMENT DTD
int checkDtdElementAndAttlistTypes(bool required, FILE* filePointer, char* type){
	if(!filePointer || !type) return 0;
    char string[characterStringSizeDtd] = {0};
	int readNumber = fread(string, 1, strlen(type), filePointer);
	if(required){
        int character = fgetc(filePointer);
        if(character != (int)' ' && character != (int)'\n' && character != (int)'\t') return 0;
	}
    if(!strcmp(string, type)) return 1;
	fseek(filePointer, -(readNumber+1), SEEK_CUR);
    return 0;
}

// VERIFIER ET RECUPERER LE ROOT(RACINE = NOM) D'UN FICHIER DTD
bool checkDtdRoot(FILE* filePointer, char** root, bool* FormatError){
	if(!filePointer) return false;
	char string[characterStringSizeDtd] = {0};
	int size = 0;
	int character = fgetc(filePointer);
	while(character != EOF && ((character >= (int)'a' && character <= (int)'z') || (character >= (int)'A' && character <= (int)'Z')|| (character >= (int)'0' && character <= (int)'9') || character == (int)'-'|| character == (int)'_' || character == (int)'.')){
 		errorLineDtdFile;
 		string[size] = (char)character;
		size++;
		character = fgetc(filePointer);
	}
	if(character != (int)'['){
        fseek(filePointer, -1, SEEK_CUR);
        skipSpacesTabsLinesBreakDtd;
        if(fgetc(filePointer) != (int)'['){
        	*FormatError = true;
            return false;
        }
    }
    fseek(filePointer, -1, SEEK_CUR);
    if(size){
        *root = (char*)malloc(sizeof(char)*(size+1));
        if(!(*root))return false;
        strcpy(*root, string); 
    }else{
        return false;
    }
	return true;
}



// ***************************************************************
// *************************CONSTRUCTEURS*************************

// ALLOUER UN NOUVEL ELEMENT DTD DE TYPE DTDELEMENT_T
dtdElement_t* dtdElementAllocation(){
    dtdElement_t* e = (dtdElement_t*)NULL;
    e = (dtdElement_t*)malloc(sizeof(dtdElement_t));
    if(!e) return (dtdElement_t*)NULL;
    e -> name = (char*)NULL;
    e -> dtdElementParameter.parameter= (char*)NULL;
    e -> nextDtdElement = (dtdElement_t*)NULL;
    e -> subDtdElement = (dtdElement_t*)NULL;
    e -> attlist = (dtdAttlist_t*)NULL;
    e -> checked = false;
    e -> errorLine = 0;
    return e;
}

// ALLOUER UN NOUVEAU DTD DE TYPE DTD_T
dtd_t* dtdAllocation(){
    dtd_t* dtd = (dtd_t*)NULL;
    dtd = (dtd_t*)malloc(sizeof(dtd_t));
    if(!dtd) return (dtd_t*)NULL;
    dtd -> root = (char*)NULL;
    dtd -> dtdElementsNumber = 0;
    dtd -> firstDtdElement = (dtdElement_t*)NULL;
    return dtd;
}



// ***************************************************************
// ********************FONCTIONS PRINCIPALES**********************

/* FONCTION PRICIPALE DE VERIFICATION DE DE RECUPERATION DE DONNEE DES ELEMENT DTD
DU FICHIER QUI FAIT DES APPELS AUX AUTRES FONCTIONS AU-DESSUS */
bool checkDtdElement(FILE* filePointer, dtd_t** dtd, dtdElement_t** element){
	if(!filePointer) return false;
    if(fgetc(filePointer)== EOF) return true;
    else fseek(filePointer, -1, SEEK_CUR);
    if(fgetc(filePointer)== (int)']'){
        fseek(filePointer, -1, SEEK_CUR);
        *element = (dtdElement_t*)NULL;
        return true;
    }
    fseek(filePointer, -1, SEEK_CUR);
    if(!checkDtdElementAndAttlistTypes(true, filePointer, "<!ELEMENT")) return false;
    skipSpacesTabsDtd;
	(*element) = dtdElementAllocation();
	if(!(*element)){
        fclose(filePointer);
        destroyDtd(dtd);
        ERROR_ALLOCATION_DTD;
    }
    bool FormatError = false;
    if(!checkDtdElementAndAttlistNames(true, filePointer, &((*element) -> name), &FormatError)){
        if(FormatError) return false;
        fclose(filePointer);
        if((*dtd) -> firstDtdElement != (*element)) destroyDtdElements(*element);
        destroyDtd(dtd);
        ERROR_ALLOCATION_DTD;
    }
    (*element) -> errorLine = LineDtdFile;
    FormatError = false;
    if(!checkDtdElementParametre(filePointer, *element, &FormatError)){
        if(FormatError) return false;
        fclose(filePointer);
        if((*dtd) -> firstDtdElement != (*element)) destroyDtdElements(*element);
        destroyDtd(dtd);
        ERROR_ALLOCATION_DTD;
    }
	if(fgetc(filePointer) != (int)'>'){
		free((*element) -> name);
		if((*element) -> dtdElementType == SUBELEMENT_TYPE_DTD && (*element) -> dtdElementParameter.parameter)free((*element) -> dtdElementParameter.parameter);
		free(*element);
        *element = (dtdElement_t*)NULL;
        return false;
	}
    skipSpacesTabsLinesBreakDtd;
	return true;
}

// VERIFIER SI UN ELEMENT DE TYPE SOUS-ELEMENT CONTIENT BIEN DES SOUS-ELEMENTS
bool lastCheck(dtdElement_t* dtdElement, FILE* filePointer){
    while(dtdElement){
        if(dtdElement -> dtdElementType == SUBELEMENT_TYPE_DTD && dtdElement -> dtdElementParameter.parameter){
            if(!dtdElement -> subDtdElement){
                LineDtdFile = dtdElement -> errorLine;
                fseek(filePointer, 0, SEEK_SET);
                return false;
            }
        }
        dtdElement = dtdElement -> nextDtdElement;
    }
    return true;
}

// VERIFIER, RECUPERER ET STOCKER UN ATTLIST
bool checkAttlist(FILE* filePointer, dtdAttlist_t** dtdAttlist){
    if(!filePointer) return false;
    bool FormatError = false;
    skipSpacesTabsLinesBreakDtd;
    char* elementName = NULL;
    if(FormatError || !checkDtdElementAndAttlistNames(true, filePointer, &elementName, &FormatError))return false;
    char*  attributName = NULL;
    skipSpacesTabsLinesBreakDtd;
    if(FormatError || !checkDtdElementAndAttlistNames(true, filePointer, &attributName, &FormatError))return false;
    skipSpacesTabsLinesBreakDtd;
    if(FormatError || !checkDtdElementAndAttlistTypes(false, filePointer, "CDATA"))return false;
    char* defaultValue = NULL;
    skipSpacesTabsLinesBreakDtd;
    if(fgetc(filePointer)==(int)'"'){
        skipSpacesTabsLinesBreakDtd;
        if(!checkDtdElementAndAttlistNames(false, filePointer, &defaultValue, &FormatError))return false;
        skipSpacesTabsLinesBreakDtd;
        if(fgetc(filePointer)!=(int)'"') return false;
    }else{
    	fseek(filePointer, -1, SEEK_CUR);
    }
    skipSpacesTabsLinesBreakDtd;
    if(fgetc(filePointer)!=(int)'>') return false;
    (*dtdAttlist) = malloc(sizeof(dtdAttlist_t));
    (*dtdAttlist) -> elementName = elementName;
    (*dtdAttlist) -> attributName = attributName;
    (*dtdAttlist) -> defaultValue = defaultValue;
    return true;
}

// VERIFIER RECUPERER LES ATTLISTS
bool getAttlist(int* sizeOfTable, FILE* filePointer, dtdAttlist_t** dtdAttlistTable, int* dtdAttlistNumber, bool* existAttlist){
   	bool toRead = false;
    do{
        if(checkDtdElementAndAttlistTypes(true, filePointer, "<!ATTLIST")){
            if(checkAttlist(filePointer, dtdAttlistTable + (*dtdAttlistNumber))){
        		(*dtdAttlistNumber)++;
        		toRead = true;
        		skipSpacesTabsLinesBreakDtd;
        		if((*dtdAttlistNumber)== (*sizeOfTable)){
        			*sizeOfTable = *sizeOfTable + 10;
   					(*dtdAttlistTable) = realloc((*dtdAttlistTable), (*sizeOfTable)*sizeof(dtdAttlist_t));
   				}
        	}

        }else{
            (*existAttlist) = false;
            toRead = false;
        }
    }while(toRead);
    return true;
}

// AJOUTER LES ATTLISTS AUX ELEMENTS ASSOCIE2
bool addAttlistsToElements(int dtdAttlistNumber, dtdAttlist_t** dtdAttlistTable, dtdElement_t* dtdElement){
	bool added = false;
	dtdElement_t* itterator = (dtdElement_t*)NULL;
	for (int i = 0; i < dtdAttlistNumber; ++i){
		itterator = dtdElement;
		while(itterator){
			if(!strcmp(dtdAttlistTable[i] -> elementName, itterator -> name)){
				if(dtdElement -> attlist) return false;
				added = true;
				itterator -> attlist = malloc(sizeof(dtdAttlist_t));
				itterator -> attlist -> elementName = malloc(sizeof(char)*(strlen(dtdAttlistTable[i] -> elementName)+1));
				itterator -> attlist -> attributName = malloc(sizeof(char)*(strlen(dtdAttlistTable[i] -> attributName)+1)); 
				strcpy(itterator -> attlist -> elementName, dtdAttlistTable[i] -> elementName);
				strcpy(itterator -> attlist -> attributName ,dtdAttlistTable[i] -> attributName);
				if(dtdAttlistTable[i] -> defaultValue){
					itterator -> attlist -> defaultValue = malloc(sizeof(char)*(strlen(dtdAttlistTable[i] -> defaultValue)+1));
					strcpy(itterator -> attlist -> defaultValue ,dtdAttlistTable[i] -> defaultValue);
				}
			}
			if(!(itterator -> nextDtdElement) && !added) return false;
			if(!(itterator -> nextDtdElement) && added){
				destroyAttlist(dtdAttlistTable[i]);
				dtdAttlistTable[i] = (dtdAttlist_t*)NULL;
			}
			itterator = itterator -> nextDtdElement;
		}		
	}

	return true;
}


/* FONCTION PRINCIPALE QUI FAIT APPEL A LA FONCTION DE DE VERIFICATION ET RECUPERATION D'ELEMENT DTD
ET AUX AUTRES FONCTIONS DE VERIFICATION */
dtd_t* checkDtd(FILE* filePointer){
    if(!filePointer) return NULL;
    dtd_t* dtd_v = (dtd_t*)NULL;
	dtd_v = dtdAllocation();
    if(!dtd_v){
        fclose(filePointer);
        ERROR_ALLOCATION_DTD;
    }
    LineDtdFile = 1;
	dtd_t** dtd = &dtd_v;
    bool noError = true;
    if(!noError || !checkDtdElementAndAttlistTypes(true, filePointer, "<!DOCTYPE")) noError = false;
    if(noError) skipSpacesTabsLinesBreakDtd;
    bool FormatError = false;
    if(!noError || !checkDtdRoot(filePointer, &((*dtd) -> root), &FormatError)) noError = false;
    if(noError) skipSpacesTabsLinesBreakDtd;
    if(!noError || !(fgetc(filePointer) == (int)'[')) noError = false;
    if(noError) skipSpacesTabsLinesBreakDtd;

    int sizeOfTable = 10;
    dtdAttlist_t* dtdAttlistTable[sizeOfTable];
    int dtdAttlistNumber = 0;
    bool existAttlist = true;
    getAttlist(&sizeOfTable, filePointer, dtdAttlistTable, &dtdAttlistNumber, &existAttlist);
    if(noError) skipSpacesTabsLinesBreakDtd;
    if(!noError || !checkDtdElement(filePointer, dtd, &((*dtd) -> firstDtdElement))) noError = false;
    dtdElement_t* itterator = (*dtd) -> firstDtdElement;
    int character = 0;
    if(itterator) (*dtd) -> dtdElementsNumber ++;
    // int dtdAttlis = dtdAttlistNumber;
    while(noError && itterator && (character = fgetc(filePointer))!=(int)']'){
        fseek(filePointer, -1, SEEK_CUR);
        skipSpacesTabsLinesBreakDtd;
        getAttlist(&sizeOfTable, filePointer, dtdAttlistTable, &dtdAttlistNumber, &existAttlist);
        itterator = (dtdElement_t*)NULL;
        skipSpacesTabsLinesBreakDtd;
        if(!noError || !checkDtdElement(filePointer, dtd, &itterator)) noError = false;
        if(itterator){
            addNextdtdElementToDtdElement((*dtd) -> firstDtdElement, itterator);
         (*dtd) -> dtdElementsNumber ++;
        }else{
        	fseek(filePointer, -2, SEEK_CUR);
            character = fgetc(filePointer);
        }
        skipSpacesTabsLinesBreakDtd;
    }
    if((*dtd) -> dtdElementsNumber == 0) character = fgetc(filePointer);
    if(character != (int)']') noError=false;
    if(noError) skipSpacesTabsLinesBreakDtd;
	if(!noError || !(fgetc(filePointer) == (int)'>')) noError = false;
	if(noError) skipSpacesTabsLinesBreakDtd;
	if(!(fgetc(filePointer)==EOF)) noError = false;
    int children = 0;
    if(noError && dtdAttlistNumber && !((*dtd) -> dtdElementsNumber)) noError = false;
    if(dtdAttlistNumber){
	if(!addAttlistsToElements(dtdAttlistNumber, dtdAttlistTable, (*dtd) -> firstDtdElement))noError = false;
	}
    if(!noError || !serachForChildren(*dtd, &children)) noError=false;
    if(!noError || !lastCheck((*dtd) -> firstDtdElement, filePointer)) noError=false;
    (*dtd) -> dtdElementsNumber -= children;
    if(!noError){
        fseek(filePointer, -2, SEEK_CUR);
        int backToLastError = fgetc(filePointer);
        while(backToLastError == (int)' ' || backToLastError == (int)'\n' || backToLastError == (int)'\t'){
            if(backToLastError == (int)'\n') LineDtdFile--;
            fseek(filePointer, -2, SEEK_CUR);
            backToLastError = fgetc(filePointer);
        }
		errorLineDtdFilePrint;
        destroyDtd(dtd);
		return (dtd_t*)NULL;
    }
	
	return dtd_v;
}



// ***************************************************************
// *********************FONCTIONS D'AFFICHAGE**********************

// FONCTION D'AFFICHAGE D'UN ELEMENT DTD
void dtdPrintElement(dtdElement_t* dtdElement){
    printf("%s  ", dtdElement-> name);
   	if(dtdElement -> attlist){
		printf("(Attribut Name : %s, Type : CDATA", dtdElement -> attlist ->  attributName);
		if(dtdElement -> attlist -> defaultValue){
			printf(", Default Value : %s) ", dtdElement -> attlist -> defaultValue);
   		}else{
   			printf(") ");
   		}	       	
	}
    if(dtdElement -> dtdElementType == PCDATA_TYPE_DTD) printf("(PCDATA Type)\n");
    else{
        printf("(Subelement Type: %s, ", dtdElement -> dtdElementParameter.parameter);       
        switch(dtdElement -> dtdElementParameter.dtdElementParameterNumber){
            case ONLY_ONE:
                            printf("Children number : one only)\n");
                            break;
            case ONE_OR_MORE:
                            printf("Children number : one or more (+))\n");
                            break;
            case ZERO_OR_MORE:
                            printf("Children number : zero or more (*))\n");
                            break;
            case ZERO_OR_ONE:
                            printf("Children number : zero or one (?))\n");
                            break;      
        }
    } 
}

// FONCTION D'AFFICHAGE D'UN DTD COMPLET
void dtdPrint(dtd_t* dtd){
	if(!dtd)return;
	printf("DTD root : %s\n", dtd -> root);
	printf("DTD elements Number : %d\n\n", dtd -> dtdElementsNumber);
	dtdElement_t* itteratorElements = dtd -> firstDtdElement;
	while(itteratorElements){
        dtdPrintElement(itteratorElements);
        if(itteratorElements -> subDtdElement){
                printf("---> ");
                dtdPrintElement(itteratorElements -> subDtdElement);
        }
        printf("\n");
        itteratorElements = itteratorElements -> nextDtdElement;
    }
}