#ifndef __MENU_H__
#define __MENU_H__

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define ERROR_ALLOCATION {printf("Erreur lors de l'allocation mémoire.\nLe programme va s'arreter.\n");exit(1);}
#define ITEMS_NUMBER 8
#define characterStringSize 100


// ***************************************************************
// **************************Structure****************************

typedef struct menu menu_t;

//	ACTION
typedef struct{
	char* actionDescription;
	void (*function)();
}action_t;

//	ITEM D'UN MENU
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
// **************************Fonctions****************************

menu_t* createMenu(const char* text);
void addMenuAction(menu_t* m, const char* string, void(*f)(), bool enabled);
void addSubMenu(menu_t* m, menu_t* sm);
void deleteMenu(menu_t* m);
void launchMenu(menu_t* m);
void setItemEnabled(menu_t* m, int number, bool enabled);


#endif