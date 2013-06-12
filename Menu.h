#ifndef MENU_H_
#define MENU_H_

#include "Arduino.h"
#include "avr/pgmspace.h"

#include "Wire.h"
#include "LiquidTWI2.h"

#define LCD_COLS 16
#define LCD_ROWS 2

#define LCD_BL_TIMEOUT	5000

// navigation
#define NAV_UP		1
#define NAV_DOWN	2
#define NAV_LEFT	3
#define NAV_RIGHT 	4
#define NAV_SELECT	5
#define NAV_NONE	0

// item types
#define TYPE_LABEL	0

#define TYPE_BYTE	1
#define TYPE_INT	2
#define TYPE_UINT	3

class MenuItem {
	PGM_P		label;
	byte		item_type;

	void 			*value;
	unsigned int	step;

public:
	MenuItem(PGM_P label, byte item_type = TYPE_LABEL, void* value = NULL, unsigned int step = 1);
	void PrintLabel(char* buffer, byte len);
	void PrintValue(char* buffer, byte len);

	// navigation
	void ChangeValue(bool increment);
	void Select();
};

class Menu {
	LiquidTWI2 lcd;

	char 		buffer[LCD_COLS+1];

	MenuItem	**items;
	byte 	nItems;

	byte 	currentItem;
	byte	currentButton;
	unsigned long	currentButtonTime;

	bool		redraw;

	byte		GetButton();

public:
	Menu(byte nItems, MenuItem** items);
	void 		Init();
	void 		Draw();
	void 		HandleNavigation();
};

#endif /* MENU_H_ */
