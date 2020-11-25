/* 
* @Author HS <hsaghir@myges.fr>
* @Developed on Nov 2020
* @Description : Description of the structure and the functions of the menu 
creation and management library
*/
#ifndef __MENU_H__
#define __MENU_H__

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define ERROR_ALLOCATION_MENU {printf("Erreur lors de l'allocation mémoire.\nLe programme va s'arreter.\n");exit(1);}
#define ITEMS_NUMBER 8


// ***************************************************************
// **************************STRUCTURE****************************

typedef struct menu menu_t;

//	ACTION
typedef struct{
	char* actionDescription;
	void (*function)();
}action_t;

//	ITEM D'UN MENU (SOUS-MENU ou ACTION)
typedef struct {
	union{
		action_t action;
		menu_t* subMenu;
	}type;
	enum {
		Action = 1,
		SubMenu
	}label;
	bool enabled;	
}item_t;

//	MENU
struct menu{
	menu_t* previousMenu;
	char* menuDescription;
	item_t	items[ITEMS_NUMBER];
	int itemsNumber;
};



// ***************************************************************
// **************************FONCTIONS****************************

// CREER UN MENU
menu_t* createMenu(const char* text);

// AJOUTER UNE ACTION A UN MENU
void addMenuAction(menu_t* m, const char* string, void(*f)(), bool enabled);

// AJOUTER UN SOUS-MENU A UN MENU
void addSubMenu(menu_t* m, menu_t* sm);

// LIBERER LA MEMOIRE OCCUPEE PAR UN MENU, SES SOUS-MENUS ET SES ACTIONS
void deleteMenu(menu_t* m);

// LANCER UN MENU
void launchMenu(menu_t* m);

// RENDRE VISIBLE ET ACTIVER UN ITEM(SOUS-MENU ou ACTION) D'UN MENU
void setItemEnabled(menu_t* m, int number, bool enabled);


#endif