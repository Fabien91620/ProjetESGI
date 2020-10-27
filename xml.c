#include "xml.h"

bool check_xml(FILE* fichier){
    if(!fichier) return false;
    if(fgetc(fichier) != (int)'<') return false;
    if(fgetc(fichier) != (int)'?') return false;
    char chaine[100];
    fgets(chaine, 4, fichier);
    if(strcmp(chaine,"xml")) return false;
    return true;
}

bool check_one_space_xml(FILE* fichier){
    if(!fichier) return false;
    if(fgetc(fichier) != (int)' ') return false;
    if(fgetc(fichier) == (int)' ') return false;
    fseek(fichier, -1,SEEK_CUR);
    return true;
}

bool check_version_xml(FILE* fichier, xml_t* xml_entree){
    if(!fichier) return false;
    char chaine[100] = {0};
    chaine[0] = (char)0;
    int taille = 0;
    int charactere = fgetc(fichier);

    if(charactere == EOF || charactere == (int)'?') return false;
    while(charactere != (int)'?'){
        if(charactere != (int)' '){
            chaine[taille] = (char)charactere;
            taille++;
        }
        charactere = fgetc(fichier);
    }
    if(fgetc(fichier) != (int)'>') return false;
    xml_entree -> version = calloc(taille,sizeof(char));
    strcpy(xml_entree -> version, chaine);
    return true;
}

bool check_retour_xml(FILE* fichier){
    if(!fichier) return false;
    if(fgetc(fichier) != 10) return false;
    return true;
}

bool check_tab_xml(FILE* fichier){
    if(!fichier) return false;
    if(fgetc(fichier) != (int)' ') return false;
    if(fgetc(fichier) != (int)' ') return false;
    if(fgetc(fichier) != (int)' ') return false;
    if(fgetc(fichier) != (int)' ') return false;
    return true;
}

bool check_parent_xml(FILE* fichier, xml_t* xml_entree, elementXml_t* element_entree){
    if(!fichier) return false;
    char chaine[100] = {0};
    chaine[0] = (char)0;
    int taille = 0;
    int charactere = fgetc(fichier);
    if(charactere != (int)'<') return false;
    charactere = fgetc(fichier);
    while(charactere != (int)'>'){
        if(charactere != (int)' '){
            chaine[taille] = (char)charactere;
            taille++;
        }
        charactere = fgetc(fichier);
    }
    element_entree -> name = calloc(taille,sizeof(char));
    strcpy(element_entree->name, chaine);
    xml_entree ->premier_fils = calloc(taille,sizeof(char));
    xml_entree ->premier_fils = element_entree;
    return true;
}

bool check_fils_xml(FILE* fichier, xml_t* xml_entree, elementXml_t* element_entree){
    if(!fichier) return false;
    char chaine[100] = {0};
    chaine[0] = (char)0;
    int taille = 0;
    int charactere = fgetc(fichier);
    if(charactere != (int)'<') return false;
    charactere = fgetc(fichier);
    while(charactere != (int)'>'){
        if(charactere != (int)' '){
            chaine[taille] = (char)charactere;
            taille++;
        }
        charactere = fgetc(fichier);
    }
//    element_entree->frere->name = calloc(taille,sizeof(char));
//    strcpy(element_entree->frere->name, chaine);
//    xml_entree ->premier_fils->frere = calloc(taille,sizeof(char));
//    xml_entree ->premier_fils->frere = element_entree->frere;
    return true;
}
