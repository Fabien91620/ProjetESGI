#include "../headers/dtd.h"

//  Numéro de la ligne d'érreur en cas d'érreur dans le fichier
int LineDtdFile = 1;

// ***************************************************************
// ****Fonctions de Libération de mémoire (Garbage Collector)****

//  Libèrer un élément DTD, ses sous-éléments et les élément suivants
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

//  Libèrer un DTD
void destroyDtd(dtd_t** dtd){
    if(!(*dtd)) return;
    destroyDtdElements((*dtd) -> firstDtdElement);
    free((*dtd) -> root);
    free(*dtd);
    (*dtd) = (dtd_t*)NULL;
}



// ***************************************************************
// ****Fonctions de vérification et de récupération de données****

//  Ignorer des charactères (exemple : \n \t ...)
void skipInDtdFile(FILE* filePointer, int toSkip){
    if(!filePointer) return;
    int character = fgetc(filePointer);
    if(toSkip == (int)'*'){
        while(character == (int)' ' || character == (int)'\n' || character == (int)'\t'){
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

//  Ajouter un élément suivant à un élément DTD (comme frere ou élément suivant)
void addNextdtdElementToDtdElement(dtdElement_t* e, dtdElement_t* elementToAdd){
    if(!e || !elementToAdd) return;
    // if(!strcmp(e -> name, elementToAdd -> name)){
    //     errorLineDtdFilePrint;
    // }
    dtdElement_t* itterator = e -> nextDtdElement;
    if(!itterator){
        e -> nextDtdElement = elementToAdd;
        return;
    }
    while(itterator -> nextDtdElement){
        // if(!strcmp(itterator -> nextDtdElement -> name, elementToAdd -> name)){
        //     errorLineDtdFilePrint;
        // }
        itterator = itterator -> nextDtdElement;
    }
    itterator -> nextDtdElement = elementToAdd;
    elementToAdd -> nextDtdElement = (dtdElement_t*)NULL;
}

//  Parcourir le DTD et mettre les éléments fils dans les éléments pères
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
            if(itteratorDtdElements -> dtdElementType == SUBELEMENT_TYPE_DTD && !strcmp(itteratorDtdElements -> dtdElementParameter.parameter, saveElement -> name)){
                if(itteratorDtdElements -> subDtdElement){
                    if(!added) addNextdtdElementToDtdElement(dtd -> firstDtdElement, saveElement);
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

//  Vérifier et récupérer le parametre d'un élément DTD
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
    }else specialCharacter = 0;
    if(!size){
        if(specialCharacter!=0){
            *FormatError = true;
            return false;
        }
        e -> dtdElementType = EMPTY_TYPE;
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

//  Vérifier et récupérer le nom d'un élément DTD
bool checkDtdElementName(FILE* filePointer, char** name, bool* FormatError){
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
	if(!size ||(character != (int)' ' && character != (int)'\n' && character != (int)'\t')){
		*FormatError = true;
		return false;
	}
	*name = (char*)malloc(sizeof(char)*(size+1));
	if(!(*name)) return false;
	strcpy(*name, string);
    return true;
}

//	Verifier le début d'un élément "ELEMENT"
bool checkDtdElementELEMENT(FILE* filePointer){
	if(!filePointer) return false;
	char string[8] = {0};
	fgets(string, 8, filePointer);
	if(strcmp(string,"ELEMENT")) return false;
	int character = fgetc(filePointer);
    if(character != (int)' ' && character != (int)'\n' && character != (int)'\t') return false;
    return true;
}

//  Vérifier et récupèrer le root
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

//  Vérifier le type du document "DOCTYPE"
bool checkDtdDOCTYPE(FILE* filePointer){
	if(!filePointer) return false;
	char string[8] = {0};
	fgets(string, 8, filePointer);
	if(strcmp(string,"DOCTYPE")) return false;
	int character = fgetc(filePointer);
    if(character != (int)' ' && character != (int)'\n' && character != (int)'\t') return false;
    errorLineDtdFile;
	return true;
}



// ***************************************************************
// *************************Constructeurs*************************

// Alouer un nouvel élément
dtdElement_t* dtdElementAllocation(){
    dtdElement_t* e = (dtdElement_t*)NULL;
    e = (dtdElement_t*)malloc(sizeof(dtdElement_t));
    if(!e) return (dtdElement_t*)NULL;
    e -> name = (char*)NULL;
    e -> dtdElementParameter.parameter= (char*)NULL;
    e -> nextDtdElement = (dtdElement_t*)NULL;
    return e;
}

// Alouer un nouveau DTD
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
// ********************Fonctions Principales**********************

// return false -> fichier non ouvert ou  erreur du format (elle libere Dtd et toutes les autres memoires)
// return true -> si element == NULL fin de lecture
//             -> si element != NULL element lu
// exit si une erreur d'allocation est survenue

//  Fontion principale de traitement d'un élément DTD (vérification et récupération)
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
	if(fgetc(filePointer)!= (int)'<' || fgetc(filePointer) != (int)'!') return false;
	if(!checkDtdElementELEMENT(filePointer)) return false;
	// skipSpacesTabsLinesBreakDtd; // juste les espaces et les tabs
	(*element) = dtdElementAllocation();
	if(!(*element)){
        fclose(filePointer);
        destroyDtd(dtd);
        ERROR_ALLOCATION_DTD;
    }
    bool FormatError = false;
    if(!checkDtdElementName(filePointer, &((*element) -> name), &FormatError)){
        if(FormatError) return false;
        fclose(filePointer);
        if((*dtd) -> firstDtdElement != (*element)) destroyDtdElements(*element);
        destroyDtd(dtd);
        ERROR_ALLOCATION_DTD;
    }
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
		if((*element) -> dtdElementType == SUBELEMENT_TYPE_DTD)free((*element) -> dtdElementParameter.parameter);
		free(*element);
	}
 	skipSpacesTabsLinesBreakDtd;
	return true;
}

/*  Fonction principale qui appelle toutes les autres fonctions pour 
vérifier et sauvegarder un filePointer DTD dans la structure */
dtd_t* checkDtd(FILE* filePointer){
    if(!filePointer) return NULL;
	dtd_t** dtd;
	(*dtd) = dtdAllocation();
    if(!(*dtd)){
        fclose(filePointer);
        ERROR_ALLOCATION_DTD;
    }
    bool noError = true;
    if(!noError || !(fgetc(filePointer) == (int)'<') || !(fgetc(filePointer) == (int)'!')) noError = false;
    if(!noError || !checkDtdDOCTYPE(filePointer)) noError = false;
    skipSpacesTabsLinesBreakDtd;
    bool FormatError = false;
    if(!noError || !checkDtdRoot(filePointer, &((*dtd) -> root), &FormatError)) noError = false;
    skipSpacesTabsLinesBreakDtd;
    if(!noError || !(fgetc(filePointer) == (int)'[')) noError = false;
    skipSpacesTabsLinesBreakDtd;
    if(!noError || !checkDtdElement(filePointer, dtd, &((*dtd) -> firstDtdElement))) noError = false;
    dtdElement_t* itterator = (*dtd) -> firstDtdElement;
    int character = 0;
    if(itterator) (*dtd) -> dtdElementsNumber ++;
    while(noError && itterator && (character = fgetc(filePointer))!=(int)']'){
        fseek(filePointer, -1, SEEK_CUR);
        itterator = (dtdElement_t*)NULL;
        skipSpacesTabsLinesBreakDtd;
        if(!noError || !checkDtdElement(filePointer, dtd, &itterator)) noError = false;
        addNextdtdElementToDtdElement((*dtd) -> firstDtdElement, itterator);
        (*dtd) -> dtdElementsNumber++;
        skipSpacesTabsLinesBreakDtd;
    }
    if((*dtd) -> dtdElementsNumber == 0) character = fgetc(filePointer);
    if(character != (int)']') noError=false;
    skipSpacesTabsLinesBreakDtd;
	if(!noError || !(fgetc(filePointer) == (int)'>')) noError = false;
	skipSpacesTabsLinesBreakDtd;
	if(!(fgetc(filePointer)==EOF)) noError = false;
    int children = 0;
    if(!noError || !serachForChildren(*dtd, &children)) noError=false;
    if(!noError){
		errorLineDtdFilePrint;
        destroyDtd(dtd);
		return (dtd_t*)NULL;
    }
    (*dtd) -> dtdElementsNumber -= children;
	return (*dtd);
}



// ***************************************************************
// *********************Fonction d'affichage**********************

// Affichage des données DTD dans la structure (pour débuguer)
void dtdPrint(dtd_t* dtd){
	if(!dtd)return;
	printf("DTD root : %s\n", dtd -> root);
	printf("DTD éléments Number : %d\n\n", dtd -> dtdElementsNumber);
	dtdElement_t* itteratorElements = dtd -> firstDtdElement;
	while(itteratorElements){
        printf("%s  ", itteratorElements-> name);
		if(itteratorElements -> dtdElementType == PCDATA_TYPE_DTD) printf("(PCDATA_TYPE_DTD)\n");
		else{
            printf("(SUBELEMENT_TYPE_DTD :   ");
            if(itteratorElements -> subDtdElement){
                printf("%s  ", itteratorElements -> subDtdElement -> name);
                if(itteratorElements -> subDtdElement -> dtdElementType == PCDATA_TYPE_DTD) printf("(PCDATA_TYPE_DTD)\n");
                else{
                    printf("(SUBELEMENT_TYPE_DTD)\n");
                    
                }
            }else{
                printf(")\n");
            } 
        }    		
		itteratorElements = itteratorElements -> nextDtdElement;
    }
}