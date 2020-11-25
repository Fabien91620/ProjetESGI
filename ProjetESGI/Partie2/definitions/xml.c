/* 
* @Authors HS <hsaghir@myges.fr>
in collaboration with 
FB  <fbarrios@myges.fr>
and 
TB <tbowetumawe@myges.fr>
* @Developed on Nov 2020
* @Description : library to check the format of an xml file and extract data
*/
#include "../headers/xml.h"

// COMPTEUR DE LA LIGNE D'ERREUR EN CAS D'ERREUR DANS LE FICHIER
int LineXmlFile = 1;

// ***************************************************************
// ****FONCTIONS DE LIBERATION DE MEMOIRE (Garbage Collector)****

// LIBERER UN ELEMENT XML, SES SOU-ELEMENTS ET LES ELEMENTS SUIVANTS
void destroyXmlElements(xmlElement_t* e){
    if(!e) return;
    destroyXmlElements(e-> subXmlElement);
    destroyXmlElements(e-> nextXmlElement);
    if(e -> xmlElementType == PCDATA_TYPE_XML) free(e -> data);
    if(e -> attlist){
        free(e -> attlist -> attributName);
        free(e -> attlist -> value);
        free(e -> attlist);
    }
    free(e -> name);
    free(e);
 }

// LIBERER UN XML DE TYPE XML_T
void destroyXml(xml_t** xml){
    if(!(*xml)) return;
    destroyXmlElements((*xml) -> firstXmlElement);
    free((*xml) -> version);
    free(*xml);
    (*xml) = (xml_t*)NULL;
}



// ***************************************************************
// ****FONTIONS DE VERIFICATION ET DE RECUPERATION DE DONNEES****

// FONCTION QUI IGNORE LES CHARACTERES D'ESPACEMENT DANS LE FICHIER XML
void skipInXmlFile(FILE* filePointer, int toSkip){
    if(!filePointer) return;
    int character = fgetc(filePointer);
    if(toSkip == (int)'*'){
        while(character == (int)' ' || character == (int)'\n' || character == (int)'\t'){
            errorLineXmlFile;
            character = fgetc(filePointer);
        }
    }else{
        while(character == toSkip){
            errorLineXmlFile;
            character = fgetc(filePointer);
        }
    }
    if(character != EOF)fseek(filePointer, -1, SEEK_CUR);
}

// AJOUTER UN ELEMENT SUIVANT A UN ELEMENT XML
void addNextXmlElementToXmlElement(xmlElement_t* e, xmlElement_t* elementToAdd){
    if(!e || !elementToAdd) return;
    xmlElement_t* itterator = e -> nextXmlElement;
    if(!itterator){
        e -> nextXmlElement = elementToAdd;
        return;
    }
    while(itterator -> nextXmlElement){
        itterator = itterator -> nextXmlElement;
    }
    itterator -> nextXmlElement = elementToAdd;
}

// AJOUTER UN SOUS-ELEMENT XML A UN ELEMENT XML
void addSubXmlElementToXmlElement(xmlElement_t* e, xmlElement_t* elementToAdd){
    if(!e || !elementToAdd || e->xmlElementType != SUBELEMENT_TYPE_XML) return;
    if(!e -> subXmlElement){
        e -> subXmlElement = elementToAdd;
        e -> childrenXmlElementNumber++;
        return;
    }
    addNextXmlElementToXmlElement(e -> subXmlElement, elementToAdd);
    e -> childrenXmlElementNumber++;
}

// VERIFIER LA BALISE FERMANT D'UN ELEMENT XML
bool checkXmlElementClosingTag(FILE* filePointer, char* stringToCompare, int size){
    if(!filePointer || !stringToCompare || !size) return false;
    if(fgetc(filePointer) != (int)'<') return false;
    if(fgetc(filePointer) != (int)'/') return false;
    char string[characterStringSizeXml] = {0};
    fgets(string, size+1, filePointer);
    if(strcmp(stringToCompare, string)) return false;
    skipSpacesTabsLinesBreakXml;
    if(fgetc(filePointer) != (int)'>') return false;
    return true;
}

// VERIFIER ET RECUPERER LES DONNEES D'UN ELEMENT XML DE TYPE #PCDATA
bool checkXmlElementData(FILE* filePointer, char** data, int* size, bool* FormatError){ 
    if(!filePointer) return false;
    char string[characterStringSizeXml] = {0};
    int character = fgetc(filePointer);
    while(character!= EOF && character != (int)'<'){
        if(character == (int)'&'){
            *FormatError = true;
            return false;
        }
        errorLineXmlFile;
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

// VERIFIER ET RECUPERER LENOM D'UN ELEMENT XML
bool checkXmlElementName(FILE* filePointer, char** name, char** attributName, char** value, bool* FormatError){
    if(!filePointer) return false;
    char string[characterStringSizeXml] = {0};
    char string2[characterStringSizeXml] = {0};
    char string3[characterStringSizeXml] = {0};
    int size = 0; int size2 = 0; int size3 = 0;
    int character = fgetc(filePointer);
    while(character != EOF && ((character >= (int)'a' && character <= (int)'z') || (character >= (int)'A' && character <= (int)'Z')|| (character >= (int)'0' && character <= (int)'9') || character == (int)'-'|| character == (int)'_' || character == (int)'.')){
        errorLineXmlFile;
        string[size] = (char)character;
        size++;
        character = fgetc(filePointer);
    }
    errorLineXmlFile;
    if(character == (int)' '){
        skipSpacesTabsLinesBreakXml;
        character = fgetc(filePointer);
            while(character != EOF && ((character >= (int)'a' && character <= (int)'z') || (character >= (int)'A' && character <= (int)'Z')|| (character >= (int)'0' && character <= (int)'9') || character == (int)'-'|| character == (int)'_' || character == (int)'.')){
                errorLineXmlFile;
                string2[size2] = (char)character;
                size2++;
                character = fgetc(filePointer);
            }
            errorLineXmlFile;
            if(character == (int)' ' || character == (int)'\n' || character == (int)'\t') character = fgetc(filePointer);
            skipSpacesTabsLinesBreakXml;
            if(character != (int)'='){
                *FormatError = true;
                return false;
            }
            skipSpacesTabsLinesBreakXml;
            if(fgetc(filePointer) != (int)'"'){
                *FormatError = true;
                return false;
            }

            character = fgetc(filePointer);
            while(character != EOF && ((character >= (int)'a' && character <= (int)'z') || (character >= (int)'A' && character <= (int)'Z')|| (character >= (int)'0' && character <= (int)'9') || character == (int)'-'|| character == (int)'_' || character == (int)'.')){
                errorLineXmlFile;
                string3[size3] = (char)character;
                size3++;
                character = fgetc(filePointer);
            }
            if(character != (int)'"'){
                *FormatError = true;
                return false;
            }
            character = fgetc(filePointer);
    } 
    if(character != (int)'>'){
        fseek(filePointer, -1, SEEK_CUR);
        skipSpacesTabsLinesBreakXml;   
        if(fgetc(filePointer) != (int)'>'){
            *FormatError = true;
            return false;
        }
    }
    if((size && size2 && size3) || (size && !size2 && !size3)){
        *name = (char*)malloc(sizeof(char)*(size+1));
        if(!(*name))return false;
        strcpy(*name, string);
        if(size2 && size3){
            *attributName = (char*)malloc(sizeof(char)*(size2+1));
            *value = (char*)malloc(sizeof(char)*(size3+1));
            strcpy(*attributName, string2);
            strcpy(*value, string3);
            if(!(*attributName) || !(*value)){
                free(*name);
                *name = (char*)NULL;
                return false;
            }
        }
    }else{
        *FormatError = true;
        return false;
    }
    return true;
}

// VERIFIER ET RECUPERER LA VERSION D'UN FICHIER XML
bool checkXmlVersion(FILE* filePointer, xml_t* xml){
    if(!filePointer || !xml) return false;
    char string1[characterStringSizeXml] = {0};
    fgets(string1, 8, filePointer);
    if(strcmp(string1,"version")) return false;
    skipSpacesTabsLinesBreakXml;
    if(fgetc(filePointer) != (int)'=') return false;
    skipSpacesTabsLinesBreakXml;
    if(fgetc(filePointer) != (int)'"') return false;
    char string2[characterStringSizeXml]={0};
    int size = 0;
    int character = fgetc(filePointer);
    if(character == EOF || character == (int)'"') return false;
    bool comma = false;
    bool characterBefor = false;
    while(character != (int)'"'){
        errorLineXmlFile;
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
        destroyXml(&xml);
        ERROR_ALLOCATION_XML;
    }
    strcpy(xml -> version, string2); 
    return true;
}

// VERIFIER LE TYPE D'UN FICHIER XML
bool checkXmlXML(FILE* filePointer){
    if(!filePointer) return false;
    char string[4] = {0};
    fgets(string, 4, filePointer);
    if(strcmp(string,"xml")) return false;
    int character = fgetc(filePointer);
    if(character != (int)' ' && character != (int)'\n' && character != (int)'\t') return false;
    errorLineXmlFile;
    return true;
}



// ***************************************************************
// *************************CONSTRUCTEURS*************************

// ALLOUER UN NOUVEL ELEMENT XML DE TYPE XELEMENT_T
xmlElement_t* xmlElementAllocation(){
    xmlElement_t* e = (xmlElement_t*)NULL;
    e = (xmlElement_t*)malloc(sizeof(xmlElement_t));
    if(!e) return (xmlElement_t*)NULL;
    e -> name = (char*)NULL;
    e -> data = (char*)NULL;
    e -> childrenXmlElementNumber = 0;
    e -> subXmlElement = (xmlElement_t*)NULL;
    e -> nextXmlElement = (xmlElement_t*)NULL;
    e -> attlist = (xmlAttlist_t*)NULL;
    e -> errorLine = 0;
    return e;
}

// ALLOUER UN NOUVEAU XML DE TYPE XML_t
xml_t* xmlAllocation(){
    xml_t* xml = (xml_t*)NULL;
    xml = (xml_t*)malloc(sizeof(xml_t));
    if(!xml) return (xml_t*)NULL;
    xml -> version = (char*)NULL;
    xml -> xmlElementsNumber = 0;
    xml -> firstXmlElement = (xmlElement_t*)NULL;
    return xml;
}



// ***************************************************************
// ********************FONCTIONS PRINCIPALES**********************

/* FONCTION PRICIPALE DE VERIFICATION DE DE RECUPERATION DE DONNEE DES ELEMENT XML
DU FICHIER QUI FAIT DES APPELS AUX AUTRES FONCTIONS AU-DESSUS */
bool checkXmlElement(FILE* filePointer, xml_t** xml, xmlElement_t** element, int depth){
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
    (*element) = xmlElementAllocation();
    if(!(*element)){
        fclose(filePointer);
        destroyXml(xml);
        ERROR_ALLOCATION_XML;
    }
    bool FormatError = false;
    char* attributName = (char*)NULL;
    char* value = (char*)NULL;
    if(!checkXmlElementName(filePointer, &((*element) -> name), &attributName, &value, &FormatError)){
        if(FormatError) return false;
        fclose(filePointer);
        if((*xml) -> firstXmlElement != (*element)) destroyXmlElements(*element);
        destroyXml(xml);
        ERROR_ALLOCATION_XML;
    }
    if(attributName){
        (*element) -> attlist = malloc(sizeof(xmlAttlist_t));{
            if(!(*element) -> attlist){
                free((*element) -> data );
                free((*element) -> name );
                return false;
            }
        }
        (*element) -> attlist -> attributName = attributName;
        (*element) -> attlist -> value = value;
    }
    (*element) -> errorLine = LineXmlFile;
    int size = 0;
    if(!checkXmlElementData(filePointer, &((*element) -> data), &size, &FormatError)){
        if(FormatError) return false;
        fclose(filePointer);
        free((*element) -> name);
        if((*xml) -> firstXmlElement != (*element)) destroyXmlElements(*element);
        destroyXml(xml);
        ERROR_ALLOCATION_XML;
    }
    if(((*element) -> data) && size){
        (*element) -> xmlElementType = PCDATA_TYPE_XML;
        if(!checkXmlElementClosingTag(filePointer, (*element) -> name, strlen((*element) -> name))){
            free((*element) -> data );
            free((*element) -> name );
            free(*element);
            return false;
        }
    }
    else{
        (*element) -> xmlElementType = SUBELEMENT_TYPE_XML;
        if(depth == 0){
            xmlElement_t* subXmlElement = (xmlElement_t*)NULL;
            if(!checkXmlElement(filePointer, xml, &subXmlElement, 1)) return false;
            while(subXmlElement){
                addSubXmlElementToXmlElement(*element, subXmlElement);
                subXmlElement = (xmlElement_t*)NULL;
                if(!checkXmlElement(filePointer, xml, &subXmlElement, 1)){
                    addSubXmlElementToXmlElement(*element, subXmlElement);
                    return false;
                }
            }
        }
        if(!checkXmlElementClosingTag(filePointer, (*element) -> name , strlen((*element) -> name))){   
            destroyXmlElements((*element));
            *element = (xmlElement_t*)NULL;
            return false;
        }
    }   
    skipSpacesTabsLinesBreakXml;
    return true;
}

/* FONCTION PRINCIPALE QUI FAIT APPEL A LA FONCTION DE DE VERIFICATION ET RECUPERATION D'ELEMENT XML
ET AUX AUTRES FONCTIONS DE VERIFICATION */
xml_t* checkXml(FILE* filePointer){
    if(!filePointer) return NULL;
    xml_t* xml_v = NULL;
    xml_v = xmlAllocation(filePointer);
    if(!xml_v){
        fclose(filePointer);
        ERROR_ALLOCATION_XML;
    }
    LineXmlFile = 1;
    xml_t** xml = &xml_v;
    bool noError = true;
    if(!noError || !(fgetc(filePointer) == (int)'<') || !(fgetc(filePointer) == (int)'?')) noError = false;
    if(!noError || !checkXmlXML(filePointer)) noError = false;
    if(noError) skipSpacesTabsLinesBreakXml;
    if(!noError || !checkXmlVersion(filePointer, *xml)) noError = false;
    if(noError) skipSpacesTabsLinesBreakXml;
    if(!noError || !(fgetc(filePointer) == (int)'?') || !(fgetc(filePointer) == (int)'>')) noError = false;
    if(noError) skipSpacesTabsLinesBreakXml;
    if(!noError || !checkXmlElement(filePointer, xml, &((*xml) -> firstXmlElement), 0)) noError = false;
    xmlElement_t* itterator = (*xml) -> firstXmlElement;
    int character, character2;
    if(itterator) xml_v -> xmlElementsNumber ++;
    while(noError && itterator && (character = fgetc(filePointer))!=(int)'>' && (character2 = fgetc(filePointer))!=EOF){
        fseek(filePointer, -2, SEEK_CUR);
        itterator = (xmlElement_t*)NULL;
        skipSpacesTabsLinesBreakXml;
        if(!noError || !checkXmlElement(filePointer, xml, &itterator, 0)) noError = false;
        addNextXmlElementToXmlElement((*xml) -> firstXmlElement, itterator);
        xml_v -> xmlElementsNumber++;
        if(noError)skipSpacesTabsLinesBreakXml;
    }
    if(!noError || (character2 == EOF && (character != (int)'>' && character != EOF))) noError = false;
    else if(!noError || (character == (int)'>' && character2 != EOF)) noError = false;
    if(!noError){
        fseek(filePointer, -2, SEEK_CUR);
        int backToLastError = fgetc(filePointer);
        while(backToLastError == (int)' ' || backToLastError == (int)'\n' || backToLastError == (int)'\t'){
            if(backToLastError == (int)'\n') LineXmlFile--;
            fseek(filePointer, -2, SEEK_CUR);
            backToLastError = fgetc(filePointer);   
        }
        errorLineXmlFilePrint;
        destroyXml(xml);
        return (xml_t*)NULL;
    }
    return xml_v;
}



// ***************************************************************
// *********************FONCTIONS D'AFFICHAGE**********************

// FONCTION D'AFFICHAGE D'UN ELEMENT XML
void printXmlElements(xmlElement_t* xmlElement){
    printf("%s  ", xmlElement-> name);
    if(xmlElement -> attlist) printf("(Attribut : %s, Value : %s) ", xmlElement -> attlist -> attributName, xmlElement -> attlist -> value);
    if(xmlElement -> xmlElementType == PCDATA_TYPE_XML) printf("(PCDATA Type : %s)\n", xmlElement -> data);
    else{
        printf("(Subelement Type, ");
        printf("Children number : %d)\n", xmlElement -> childrenXmlElementNumber); 
    }
}

// FONCTION D'AFFICHAGE D'UN XML COMPLET
void printXml(xml_t* xml){
    if(!xml)return;
    printf("Version : %s\n", xml -> version);
    printf("Elements number : %d\n\n", xml -> xmlElementsNumber);
    xmlElement_t* itteratorElement = xml -> firstXmlElement;
    while(itteratorElement){
        printXmlElements(itteratorElement);
        xmlElement_t* itteratorSubElement = itteratorElement -> subXmlElement;
        while(itteratorSubElement){
            printf("---> ");
            printXmlElements(itteratorSubElement);
            itteratorSubElement = itteratorSubElement -> nextXmlElement;
        }
        printf("\n");
        itteratorElement = itteratorElement -> nextXmlElement; 
    }
}