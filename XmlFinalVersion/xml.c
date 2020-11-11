#include "xml.h"

//  Numéro de la ligne d'érreur en cas d'érreur dans le fichier
int LineXmlFile = 1;

// ***************************************************************
// ****Fonctions de Libération de mémoire (Garbage Collectors)****

//  Libèrer un élément, ses sous-éléments et les élément suivants
void destroyElements(xmlElement_t* e){
    if(!e) return;
    if(e -> subXmlElement && e -> elementType == SUBELEMENT_TYPE){
        destroyElements(e-> subXmlElement);
    }
    if(e -> nextElement){
        destroyElements(e-> nextElement);
    }
    if(e -> elementType == PCDATA_TYPE){
        free(e -> data);
        e -> data = (char*)NULL;
    }
    free(e -> name);
    e -> name = (char*)NULL;
    free(e);
    e -> subXmlElement = (xmlElement_t*)NULL;
    e -> nextElement = (xmlElement_t*)NULL;
    e = (xmlElement_t*)NULL;              
}

//  Libèrer un xml
void destroy(xml_t** xml){
    if(!(*xml)) return;
    if((*xml) -> firstXmlElement){
        destroyElements((*xml) -> firstXmlElement);
        (*xml) -> firstXmlElement = (xmlElement_t*)NULL;
    }
    free((*xml) -> version);
    free(*xml);
    (*xml) = (xml_t*)NULL;
}



// ***************************************************************
// ****Fonctions de vérification et de récupération de données****

//  Ignorer des charactères (exemple : \n \t ...)
void skip(FILE* filePointer, int toSkip){
    if(!filePointer) return;
    int character = fgetc(filePointer);
    if(toSkip == (int)'*'){
        while(character == (int)' ' || character == (int)'\n' || character == (int)'\t'){
            errorLineFile;
            character = fgetc(filePointer);
        }
    }else{
        while(character == toSkip){
            errorLineFile;
            character = fgetc(filePointer);
        }
    }
    if(character != EOF)fseek(filePointer, -1, SEEK_CUR);
}

//  Ajouter un sous élément à un élément (comme fils)
void addNextXmlElementToXmlElement(xmlElement_t* e, xmlElement_t* elementToAdd){
    if(!e || !elementToAdd) return;
    xmlElement_t* itterator = e -> nextElement;
    if(!itterator){
        e -> nextElement = elementToAdd;
        return;
    }
    while(itterator -> nextElement){
        itterator = itterator -> nextElement;
    }
    itterator -> nextElement = elementToAdd;
}

//  Ajouter un sous élément à un élément (comme fils)
void addSubXmlElementToXmlElement(xmlElement_t* e, xmlElement_t* elementToAdd){
    if(!e || !elementToAdd || e->elementType != SUBELEMENT_TYPE) return;
    if(!e -> subXmlElement){
        e -> subXmlElement = elementToAdd;
        e -> childrenNumber++;
        return;
    }
    addNextXmlElementToXmlElement(e -> subXmlElement, elementToAdd);
    e -> childrenNumber++;
}

//  Vérifier une balise fermante (exemple : </Classrooms>)
bool checkClosingTag(FILE* filePointer, char* stringToCompare, int size){
    if(!filePointer || !stringToCompare || !size) return false;
    if(fgetc(filePointer) != (int)'<') return false;
    if(fgetc(filePointer) != (int)'/') return false;
    char string[characterStringSize] = {0};
    fgets(string, size+1, filePointer);
    if(strcmp(stringToCompare, string)) return false;
    skipSpacesTabsLinesBreak;
    if(fgetc(filePointer) != (int)'>') return false;
    return true;
}

//  Vérifier et récupèrer les données d'un élément de type PCDATA
bool checkXmlElementData(FILE* filePointer, char** data, int* size){ 
    if(!filePointer) return false;
    char string[characterStringSize] = {0};
    int character = fgetc(filePointer);
    while(character!= EOF && character != (int)'<'){
        if(character == (int)'&') return false;
        errorLineFile;
        string[*size] = (char)character;
        if(character != (int)' ' && character != (int)'\n' && character != (int)'\t')(*size)++;
        character = fgetc(filePointer);
    }
    fseek(filePointer, -1, SEEK_CUR);
    if(*size){
        *data = (char*)malloc(sizeof(char)*((*size)+1));
        if(!*data) return false;
        strcpy(*data, string); 
    }
    return true;
}

//  Vérifier et récupérer le nom d'un élément
bool checkElementName(FILE* filePointer, char** name){
    char string[characterStringSize] = {0};
    int size = 0;
    int character = fgetc(filePointer);
    while(character != EOF && ((character >= (int)'a' && character <= (int)'z') || (character >= (int)'A' && character <= (int)'Z') || character == (int)'-'|| character == (int)'_' || character == (int)'.')){
        errorLineFile;
        string[size] = (char)character;
        size++;
        character = fgetc(filePointer);
    }
    errorLineFile;
    if(character != (int)'>'){
        skipSpacesTabsLinesBreak;   
        if(fgetc(filePointer) != (int)'>') return false;
    }
    if(size){
        *name = (char*)malloc(sizeof(char)*(size+1));
        if(!*name)return false;
        strcpy(*name, string); 
    }else{
        return false;
    }
    return true;
}

//  Vérifier et récupérer la version (exemple "1." ou "1.0" ou "2.0" -> valides , ".0" ou "1.2.1" non-valides)
bool checkXmlVersion(FILE* filePointer, xml_t* xml){
    if(!filePointer || !xml) return false;
    char string1[8] = {0};
    fgets(string1, 8, filePointer);
    if(strcmp(string1,"version")) return false;
    skipSpacesTabsLinesBreak;
    if(fgetc(filePointer) != (int)'=') return false;
    skipSpacesTabsLinesBreak;
    if(fgetc(filePointer) != (int)'"') return false;
    char string2[characterStringSize]={0};
    int size = 0;
    int character = fgetc(filePointer);
    if(character == EOF || character == (int)'"') return false;
    bool comma = false;
    bool characterBefor = false;
    while(character != (int)'"'){
        errorLineFile;
        if(character!=(int)'.' && (character > (int)'9' || character < (int)'0')) return false;
        if((character==(int)'.' && !characterBefor) || (character==(int)'.' && comma)) return false;
        if(character == (int)'.') comma = true;
        string2[size] = (char)character;
        size++;
        characterBefor = true;
        character = fgetc(filePointer);
    }
    xml -> version = (char*)malloc(sizeof(char)*(size+1));
    if(!xml -> version){
        fclose(filePointer);
        destroy(&xml);
        ERROR_ALLOCATION;
    }
    strcpy(xml -> version, string2); 
    return true;
}

//  Vérifier le type du document "xml"
bool checkXmlXML(FILE* filePointer){
    if(!filePointer) return false;
    char string[characterStringSize];
    fgets(string, 4, filePointer);
    if(strcmp(string,"xml")) return false;
    int character = fgetc(filePointer);
    if(character != (int)' ' && character != (int)'\n' && character != (int)'\t') return false;
    errorLineFile;
    return true;
}



// ***************************************************************
// *************************Constructeurs*************************

// Alouer un nouvel élément
xmlElement_t* xmlElementAllocation(){
    xmlElement_t* e = (xmlElement_t*)NULL;
    e = (xmlElement_t*)malloc(sizeof(xmlElement_t));
    if(!e) return (xmlElement_t*)NULL;
    e -> name = (char*)NULL;
    e -> data = (char*)NULL;
    e -> childrenNumber = 0;
    e -> subXmlElement = (xmlElement_t*)NULL;
    e -> nextElement = (xmlElement_t*)NULL;
    return e;
}

// Alouer un nouveau xml
xml_t* xmlAllocation(){
    xml_t* xml = (xml_t*)NULL;
    xml = (xml_t*)malloc(sizeof(xml_t));
    if(!xml) return (xml_t*)NULL;
    xml -> version = (char*)NULL;
    xml -> elementsNumber = 0;
    xml -> firstXmlElement = (xmlElement_t*)NULL;
    return xml;
}



// ***************************************************************
// ********************Fonctions Principales**********************

//  Fontion principale de traitement d'un élément (vérification et récupération)
bool check_element(FILE* filePointer, xml_t** xml, xmlElement_t** element, int depth){
    if(!filePointer) return false;
    if(fgetc(filePointer)== EOF) return true;
    else{
        fseek(filePointer, -1, SEEK_CUR);
    }
    if(fgetc(filePointer)!= (int)'<'){
        fseek(filePointer, -1, SEEK_CUR);
        return false;
    }
    if(fgetc(filePointer)== (int)'/'){
        fseek(filePointer, -2, SEEK_CUR);
        *element = (xmlElement_t*)NULL;
        return true;
    }
    fseek(filePointer, -1, SEEK_CUR);
    (*element) = xmlElementAllocation(filePointer, xml);
    if(!(*element)){
        fclose(filePointer);
        destroy(xml);
        ERROR_ALLOCATION;
    }
    char* name = (char*)NULL;
    if(!checkElementName(filePointer, &name)){
        fclose(filePointer);
        if((*xml) -> firstXmlElement != (*element)) destroyElements(*element);
        destroy(xml);
        ERROR_ALLOCATION;
    }
    char* data = (char*)NULL;
    int size = 0;
    if(!checkXmlElementData(filePointer, &data, &size)){
        fclose(filePointer);
        free(name);
        if((*xml) -> firstXmlElement != (*element)) destroyElements(*element);
        destroy(xml);
        ERROR_ALLOCATION;
    }
    (*element) -> name = name;
    (*element) -> data = data;
    if(data && size){
        (*element) -> elementType = PCDATA_TYPE;
        if(!checkClosingTag(filePointer, (*element) -> name = name, strlen((*element) -> name = name))){
            free((*element) -> data );
            free((*element) -> name );
            free(*element);
            return false;
        }
    }
    else{
        (*element) -> elementType = SUBELEMENT_TYPE;
        if(depth == 0){
            xmlElement_t* subXmlElement = (xmlElement_t*)NULL;
            if(!check_element(filePointer, xml, &subXmlElement, 1)) return false;
            while(subXmlElement){
                addSubXmlElementToXmlElement(*element, subXmlElement);
                subXmlElement = (xmlElement_t*)NULL;
                if(!check_element(filePointer, xml, &subXmlElement, 1)) return false;
            }
        }
        if(!checkClosingTag(filePointer, (*element) -> name , strlen((*element) -> name))){   
            destroyElements((*element));
            *element = (xmlElement_t*)NULL;
            return false;
        }
    }   
    skipSpacesTabsLinesBreak;
    return true;
}

/*  Fonction principale qui appelle toutes les autres fonctions pour 
vérifier et sauvegarder un fichier xml dans la structure */
xml_t* checkXml(FILE* filePointer){
    if(!filePointer) return false;
    xml_t* xml_v = xmlAllocation(filePointer);
    if(!xml_v){
        fclose(filePointer);
        ERROR_ALLOCATION;
    }
    xml_t** xml = &xml_v;
    bool noError = true;
    if(!noError || !fgetc(filePointer) == (int)'<' || !fgetc(filePointer) == (int)'?') noError = false;
    if(!noError || !checkXmlXML(filePointer)) noError = false;
    skipSpacesTabsLinesBreak;
    if(!noError || !checkXmlVersion(filePointer, *xml)) noError = false;
    skipSpacesTabsLinesBreak;
    if(!noError || !fgetc(filePointer) == (int)'?' || !fgetc(filePointer) == (int)'>') noError = false;
    skipSpacesTabsLinesBreak;
    if(!noError || !check_element(filePointer, xml, &((*xml) -> firstXmlElement), 0)) noError = false;
    xmlElement_t* itterator = (*xml) -> firstXmlElement;
    int character, character2;
    if(itterator) xml_v -> elementsNumber ++;
    while(itterator && (character = fgetc(filePointer))!=(int)'>' && (character2 = fgetc(filePointer))!=EOF){
        fseek(filePointer, -2, SEEK_CUR);
        itterator = (xmlElement_t*)NULL;
        skipSpacesTabsLinesBreak;
        if(!noError || !check_element(filePointer, xml, &itterator, 0)) noError = false;
        addNextXmlElementToXmlElement((*xml) -> firstXmlElement, itterator);
        xml_v -> elementsNumber ++;
        skipSpacesTabsLinesBreak;
    }
    if(character2 == EOF && (character != (int)'>' && character != EOF)) noError = false;
    else if(character == (int)'>' && character2 != EOF) noError = false;
    if(!noError){
        errorLineFilePrint;
        destroy(xml);
        return (xml_t*)NULL;
    }
    return xml_v;
}



// ***************************************************************
// *********************Fonction d'affichage**********************

// Affichage de la structure (pour débuguer)
void xmlPrint(xml_t* xml){
    if(!xml)return;
    printf("Version : %s\n", xml -> version);
    printf("Elements number : %d\n\n", xml -> elementsNumber);
    xmlElement_t* itteratorElement = xml -> firstXmlElement;
     xmlElement_t* itteratorSubElement;
    while(itteratorElement){
        printf("%s  ", itteratorElement-> name);
        if(itteratorElement -> elementType == PCDATA_TYPE) printf("(PCDATA_TYPE = %s)\n", itteratorElement -> data);
        else{
            printf("(SUBELEMENT_TYPE)  ");
            printf("children number = %d\n", itteratorElement -> childrenNumber); 
            itteratorSubElement = itteratorElement -> subXmlElement;
            while(itteratorSubElement){
                printf("\tname = %s  ", itteratorSubElement -> name);
                if(itteratorSubElement -> elementType == PCDATA_TYPE) printf("(PCDATA_TYPE = %s)\n", itteratorSubElement -> data);
                else{
                    printf("(SUBELEMENT_TYPE)  ");
                    printf("children number = %d\n", itteratorSubElement -> childrenNumber);   
                }
                itteratorSubElement = itteratorSubElement -> nextElement;
            }    
        }
        itteratorElement = itteratorElement -> nextElement; 
    }
}