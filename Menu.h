#ifndef MENU_H_
#define MENU_H_

#include "Arduino.h"
#include "avr/pgmspace.h"

#include "Wire.h"
#include "Adafruit_RGBLCDShield.h"

#define RED 0x1
#define YELLOW 0x3
#define GREEN 0x2
#define TEAL 0x6
#define BLUE 0x4
#define VIOLET 0x5
#define WHITE 0x7

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
	void PrintLabel(char* buffer, byte cols);
	void PrintValue(char* buffer, byte cols);

	// navigation
	void ChangeValue(bool increment);
	void Select();
};

class Menu {
	Adafruit_RGBLCDShield lcd;

	char* 		buffer;

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
