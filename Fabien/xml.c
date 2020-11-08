#include "xml.h"

bool check_element_data(FILE* fichier, char** data){ 
    if(!fichier) return false;
    char chaine[100] = {0};
    int taille = 0;
    int charactere = fgetc(fichier);
    while(charactere!= EOF && charactere != (int)'<'){
        if(charactere != (int)' ' && charactere != (int)'\n' && charactere != (int)'\t'){
            chaine[taille] = (char)charactere;
            taille++;
        }
        charactere = fgetc(fichier);
    }
    fseek(fichier, -1, SEEK_CUR);
    if(taille){
        *data = malloc(sizeof(char)*(taille+1));
        if(!*data)ERROR_ALLOCATION;
        strcpy(*data, chaine); 
    }
    return true;
}


void add_subelement_to_element(elementXml_t* e_dest, elementXml_t* element){
    if(!e_dest || !element || e_dest->type != SOUS_ELEMENT) return;
    elementXml_t* itterator = e_dest -> premier_fils;
    if(!itterator){
        e_dest -> premier_fils = element;
        return;
    }
    while(itterator -> premier_frere){
        itterator = itterator -> premier_frere;
    }
    itterator -> premier_frere = element;
}


bool check_balises_close(FILE* fichier, char* chaine, int taille){
    if(!fichier || !chaine) return false;
    if(fgetc(fichier) != (int)'<') return false;
        if(fgetc(fichier) != (int)'/') return false;
        char chaine2[100] = {0};
        fgets(chaine2, taille+1, fichier);
        if(strcmp(chaine, chaine2)) {
            return false;
        }
        if(fgetc(fichier) != (int)'>') return false;
        return true;
}


bool check_element(FILE* fichier, xml_t* xml, elementXml_t** element){
    if(!fichier) return false;
    if(fgetc(fichier)!= (int)'<'){
        fseek(fichier, -1, SEEK_CUR);
        return false;
    }
    if(fgetc(fichier)== (int)'/'){
        fseek(fichier, -2, SEEK_CUR);
        *element = NULL;
        return true;
    }
    fseek(fichier, -1, SEEK_CUR);
    char chaine[100] = {0};
    int taille = 0;
    int charactere = fgetc(fichier);
    while(charactere != (int)'>'){
        if(charactere == ' ') return false;
        chaine[taille] = (char)charactere;
        taille++;
        charactere = fgetc(fichier);
    }
    char* data = NULL;
    if(!check_element_data(fichier, &data)) return false;


    // Imagine qu'il n'y a pas qu'un seul element pere - il faut traiter ce cas
    (*element) = malloc(sizeof(elementXml_t));

    if(!*element){
        ERROR_ALLOCATION;
        free(data);
        exit(1);
    }
    (*element) -> name = NULL;
    (*element) -> name = malloc(sizeof(char) * (taille + 1));
    if(!(*element) -> name){
        ERROR_ALLOCATION;
        free(data);
        free(*element);
        exit(1);
    }
    strcpy( (*element) -> name, chaine);
    if(data){
        (*element) -> type = PCDATA;
        (*element) -> data = data;
        if(!(*element) -> data){
            ERROR_ALLOCATION;
            free((*element) -> data);
            free((*element) -> name);
            free(*element);
            exit(1);
        }
        if(!check_balises_close(fichier, chaine, taille)){
            free((*element) -> data );
            free((*element) -> name );
            free(*element);
            return false;
        }
        if(fgetc(fichier) != (int)'\n') return false; //
    }
    else{
        (*element) -> type = SOUS_ELEMENT;
        elementXml_t* subelement = NULL;
        if(!check_element(fichier, xml, &subelement)){
            return false;
        }
        while(subelement){
            add_subelement_to_element(*element, subelement);
            
            subelement = (elementXml_t*)NULL;
            if(!check_element(fichier, xml, &subelement)){
                return false;
            }
        }
        if(!check_balises_close(fichier, chaine, taille)) return false;
    }   
    return true;
}

bool check_xml_version(FILE* fichier, xml_t* xml){
    if(!fichier) return false;
    if(!xml) return false;
    char chaine0[8] = {0};
    fgets(chaine0, 8, fichier);
    if(strcmp(chaine0,"version")) return false;
    if(fgetc(fichier) != (int)'=') return false;
    if(fgetc(fichier) != (int)'"') return false;
    char chaine[100]={0};
    int taille = 0;
    int charactere = fgetc(fichier);
    if(charactere == EOF || charactere == (int)'"') return false;
    while(charactere != (int)'"'){
        if(charactere == (int)' ') return false;
        chaine[taille] = (char)charactere;
        taille++;
        charactere = fgetc(fichier);
    }
    xml -> version = malloc(sizeof(char)*(taille+1));
    if(!xml -> version) ERROR_ALLOCATION;
    strcpy(xml -> version, chaine); 
    return true;
}

bool check_xml_XML(FILE* fichier){
    if(!fichier) return false;
    char chaine[100];
    fgets(chaine, 4, fichier);
    if(strcmp(chaine,"xml")) return false;
    if(fgetc(fichier) != (int)' ') return false;
    return true;
}


bool check_xml(FILE* fichier, xml_t* xml){
    if(!fichier) return false;
    if(!xml) return false;
    bool RETOUR = true;
    if(fgetc(fichier) != (int)'<') RETOUR = false;
    if(!RETOUR || !fgetc(fichier) == (int)'?') RETOUR = false;
    if(!RETOUR || !check_xml_XML(fichier)) RETOUR = false;
    if(!RETOUR || !check_xml_version(fichier, xml)) RETOUR = false;
    if(!RETOUR || !fgetc(fichier) == (int)'?') RETOUR = false;
    if(!RETOUR || !fgetc(fichier) == (int)'>') RETOUR = false;
    if(!RETOUR || !fgetc(fichier) == (int)'\n') RETOUR = false;
    if(!RETOUR){
        destroy(xml);
        exit(1);
    }
    if(!check_element(fichier, xml, &xml -> premier_fils)) return false;
    return true;
}


void afficheXML(xml_t* xml){
    if(!xml)return;
    printf("Version : %s\n", xml -> version);
    elementXml_t* itterator_Classrooms = xml -> premier_fils;
     elementXml_t* itterator_Classroom;
    if(itterator_Classrooms){
        printf("%s\n", itterator_Classrooms-> name);
        if(itterator_Classrooms -> type == PCDATA) printf("%s\n", itterator_Classrooms-> data);
        else{
            itterator_Classroom = itterator_Classrooms -> premier_fils;
            while(itterator_Classroom){
                printf("\t%s ", itterator_Classroom -> name);
                if(itterator_Classroom -> type == PCDATA){
                    printf("%s\n", itterator_Classroom-> data);
                    itterator_Classroom = itterator_Classroom -> premier_frere;
                }
            }    
        }
        // itterator_Classrooms = itterator_Classrooms -> premier_frere; 
    }
}

void destroy_elements(elementXml_t* e){
    if(!e) return;
    if(e -> premier_fils && e -> type == SOUS_ELEMENT){
        destroy_elements(e-> premier_fils);
    }
    if(e -> premier_frere){
        destroy_elements(e-> premier_frere);
    }
    if(e -> type == PCDATA){
        free(e -> data);
    }
        free(e -> name);
        free(e);
}

void destroy(xml_t* xml){
    if(!xml) return;
    if(xml -> premier_fils) destroy_elements(xml -> premier_fils);
    free(xml -> version);
    free(xml);
}
