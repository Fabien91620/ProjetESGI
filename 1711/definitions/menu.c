#include "../headers/menu.h"

//	Créer un menu
menu_t* createMenu(const char* string){
	menu_t* m = (menu_t*)NULL;
	m = (menu_t*)malloc(sizeof(menu_t));
	if(!m)ERROR_ALLOCATION;
	m -> previousMenu = (menu_t*)NULL;
	m -> menuDescription = (char*)malloc(sizeof(strlen(string)+1));
	if(!m -> menuDescription){
		free(m);
		ERROR_ALLOCATION;
	}
	strcpy(m -> menuDescription, string);
	m -> itemsNumber = 0;
	return m;
}

//	Libérer la mémoire occupée par un menu
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

//	Créer et ajouter une action à un menu
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
		ERROR_ALLOCATION;
	}
	strcpy(action.actionDescription, string);
	action.function = f;
	m -> items[m->itemsNumber].type.action = action;
	m -> items[m->itemsNumber].label = Action;
	m -> items[m->itemsNumber].enabled = enabled;
	m -> itemsNumber++;
}

//	Ajouter un sous-menu à un menu
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

//	Afficher ou non un item d'un menu
void setItemEnabled(menu_t* m, int number, bool enabled){
	m -> items[number-1].enabled = enabled;
}

//	Lancer un menu
void launchMenu(menu_t* m){
	printf("%s :\n", m -> menuDescription);
	for(int i=0; i < m -> itemsNumber; i++){
		if(m -> items[i].enabled == true){
			if(m -> items[i].label == Action) printf("%d - %s\n",i+1, m -> items[i].type.action.actionDescription);
			else printf("%d - %s\n",i+1, m -> items[i].type.subMenu -> menuDescription);			
		}
	}
	int key = 0;
	printf("\nchoice ? ");
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