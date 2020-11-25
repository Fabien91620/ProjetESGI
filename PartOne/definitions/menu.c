/* 
* @Author HS <hsaghir@myges.fr>
* @Developed on Nov 2020
* @Description : menu creation and management library
*/
#include "../headers/menu.h"

// CREER UN MENU
menu_t* createMenu(const char* string){
	menu_t* m = (menu_t*)NULL;
	m = (menu_t*)malloc(sizeof(menu_t));
	if(!m)ERROR_ALLOCATION_MENU;
	m -> previousMenu = (menu_t*)NULL;
	m -> menuDescription = (char*)malloc(sizeof(strlen(string)+1));
	if(!m -> menuDescription){
		free(m);
		ERROR_ALLOCATION_MENU;
	}
	strcpy(m -> menuDescription, string);
	m -> itemsNumber = 0;
	return m;
}

// LIBERER LA MEMOIRE OCCUPEE PAR UN MENU
void deleteMenu(menu_t* m){
	if(!m) return;
	if(m -> previousMenu != NULL){
		printf("Vous ne pouvez pas supprimer ce menu.\n");
		return;
	}
	for(int i=0; i<m -> itemsNumber; i++){
		if(m -> items[i].label == Action){
			free(m -> items[i].type.action.actionDescription);
		}else{
			m->items[i].type.subMenu -> previousMenu = (menu_t*)NULL;
			deleteMenu(m -> items[i].type.subMenu);
		}
	}
	free(m -> menuDescription);
	free(m);
}

// CREER ET AJOUTER UNE ACTION A UN MENU DONNE EN PARAMETRE
void addMenuAction(menu_t* m, const char* string, void(*f)(), bool enabled){
	if(!m) return;
	if(m->itemsNumber>= ITEMS_NUMBER){
		printf("Vous ne pouvez plus ajouter des items a ce menu.\n");
		return;
	}
	action_t action;
	action.actionDescription = NULL;
	action.actionDescription = (char*)malloc(sizeof(char)*strlen(string)+1);
	if(!action.actionDescription){
		deleteMenu(m);
		ERROR_ALLOCATION_MENU;
	}
	strcpy(action.actionDescription, string);
	action.function = f;
	m -> items[m->itemsNumber].type.action = action;
	m -> items[m->itemsNumber].label = Action;
	m -> items[m->itemsNumber].enabled = enabled;
	m -> itemsNumber++;
}

// AJOUTER UN SOUS-MENU A UN MENU DONNE EN PARAMTERE
void addSubMenu(menu_t* m, menu_t* menuToAdd){
	if(menuToAdd -> previousMenu != NULL){
		printf("Vous ne pouvez pas ajouter ce sous-menu a ce menu.\n");
		return;
	}
	menuToAdd -> previousMenu = m;
	m -> items[m->itemsNumber].type.subMenu = menuToAdd;
	m -> items[m->itemsNumber].label = SubMenu;
	m -> items[m->itemsNumber].enabled = true;
	m -> itemsNumber++;
}

// MODIFIER LA VARIABLE D'ACTIVATION D'UN ITEM(ACTION OU SOUS-MENU) D'UN MENU
void setItemEnabled(menu_t* m, int number, bool enabled){
	m -> items[number-1].enabled = enabled;
}

// LANCER UN MENU
void launchMenu(menu_t* m){
	printf("%s :\n", m -> menuDescription);
	for(int i=0; i < m -> itemsNumber; i++){
		if(m -> items[i].enabled == true){
			if(m -> items[i].label == Action) printf("%d - %s\n",i+1, m -> items[i].type.action.actionDescription);
			else printf("%d - %s\n",i+1, m -> items[i].type.subMenu -> menuDescription);			
		}
	}
	int key = 0;
	printf("\nchoix ? ");
	while((key < (int)'1' || key > ((m -> itemsNumber)+(int)'0') || m -> items[key-49].enabled != true) && key!=(int)'p'){
		key = getchar();
	}
	switch(key){
		case (int)'p' :	if(m->previousMenu)launchMenu(m->previousMenu);
						break;
		default :		if(m -> items[key-49].enabled == true){
							if(m -> items[key-49].label == Action)	m -> items[key-49].type.action.function();
							else	launchMenu(m -> items[key-49].type.subMenu);
						}
						break;
	}
}