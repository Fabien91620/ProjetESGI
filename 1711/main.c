#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "headers/xml.h"
#include "headers/dtd.h"
#include "headers/menu.h"

bool ON = true;
xml_t* xml = (xml_t*)NULL;  //  Pointeur vers la structure XML chargée à partir du fichier XML
dtd_t* dtd;  //  Pointeur vers la structure DTD chargée à partir du fichier DTD

menu_t* m = (menu_t*)NULL;  //  Menu

//  Charger, vérifier et sauvegarder un fichier xml dans la structure
void xmlLoad() {
    system("clear");
    if(xml){
        printf(">> Les anciennes données XML sauvegardées ont été ecrasées.\n\n");
        setItemEnabled(m, 4, false);
        destroyXml(&xml);
        xml = (xml_t*)NULL;
    }
    printf(">> Veuillez entrer le nom du fichier XML à charger: \n(Le fichier doit se trouver dans le répértoire de cette application)\n\n");
    char string[100] = {0};
    scanf("%s", string);
    FILE* xmlFilePointer = (FILE*)NULL;
    xmlFilePointer = fopen(string, "r");
    system("clear");
    if(!xmlFilePointer){
        printf(">> Erreur lors de l'ouverture du ficher \"%s\".\n(Vérifier le nom et le chemin de votre fichier)\n\n", string);
        return;
    }
    xml = checkXml(xmlFilePointer);
    if(xml){
        printf(">> XML : Format correct.\nLes données du fichier \"%s\" ont été sauvegardées.\n", string);
        setItemEnabled(m, 4, true);
        
    }else{
        printf(">> XML : Format incorrect.\nLes données du fichier \"%s\" n'ont PAS été sauvegardées.\n", string);
    }
    fclose(xmlFilePointer);
}
 
//***  Charger, vérifier et sauvegarder un fichier dtd dans la structure
void dtdLoad() {
    system("clear");
    if(dtd){
        printf(">> Les anciennes données DTD sauvegardées ont été ecrasées.\n\n");
        setItemEnabled(m, 5, false);
        destroyDtd(&dtd);
        dtd = (dtd_t*)NULL;
    }
    printf(">> Veuillez entrer le nom du fichier DTD à charger: \n(Le fichier doit se trouver dans le répértoire de cette application)\n\n");
    char string[100] = {0};
    scanf("%s", string);
    FILE* dtdFilePointer = (FILE*)NULL;
    dtdFilePointer = fopen(string, "r");
    system("clear");
    if(!dtdFilePointer){
        printf(">> Erreur lors de l'ouverture du ficher \"%s\".\n(Vérifier le nom et le chemin de votre fichier)\n\n", string);
        return;
    }
    dtd = checkDtd(dtdFilePointer);
    if(dtd){
        printf(">> DTD : Format correct.\nLes données du fichier \"%s\" ont été sauvegardées.\n", string);
        setItemEnabled(m, 5, true);
    }
    else{
        printf(">> DTD : Format incorrect.\nLes données du fichier \"%s\" n'ont PAS été sauvegardées.\n", string);
    }
    fclose(dtdFilePointer);
}



//***  Vérifier la conformité d'un fichier xml avec son fichier dtd
// void compareXmlToDtd(){
// }

//  Afficher données du fichier xml
void xmlPrintData() {
    system("clear");
    printXml(xml);
    printf("\n\n");
}
 
//***  Afficher données du fichier dtd
void dtdPrintData() {
    system("clear");
    dtdPrint(dtd);
    printf("\n\n");
}

void quit() {
    ON = false;
}

// int main(int argc, char** argv) {
 
int main() {
    system("clear"); system("clear"); 
    m = createMenu("Main menu");
    addMenuAction(m,"Charger et vérifier un fichier XML",xmlLoad, true); //1
    addMenuAction(m,"Charger et vérifier un fichier DTD",dtdLoad, true);       //2
    // addMenuAction(m,"Vérifier la conformité DTD-XML",NULL, false);  //3
    addMenuAction(m,"Afficher les données XML sauvegardées",xmlPrintData, false);  //4
    addMenuAction(m,"Afficher les données DTD sauvegardées",dtdPrintData, false);  //5
    addMenuAction(m,"Quitter",quit, true);                                 //6
    
    while(ON) launchMenu(m);
    deleteMenu(m);
    if(xml) destroyXml(&xml);
    if(dtd) destroyDtd(&dtd);
    return 0;
}