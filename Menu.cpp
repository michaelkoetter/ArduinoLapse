#include "Menu.h"

MenuItem::MenuItem(PGM_P label, uint8_t item_type, void* value, unsigned int step)
: label(label), value(value), item_type(item_type), step(step)
{
}

void MenuItem::PrintLabel(char* buffer, byte cols) {
	strncpy_P(buffer, label, cols);
}

void MenuItem::PrintValue(char* buffer, byte cols) {
	switch(item_type) {
	case TYPE_INT:
		snprintf(buffer, cols, "%i", *((int*)value));
		break;
	case TYPE_UINT:
		snprintf(buffer, cols, "%u", *((unsigned int*)value));
		break;
	}
}

void MenuItem::ChangeValue(bool increment) {
	switch(item_type) {
	case TYPE_INT:
		*((int*)value) += step * (increment ? 1 : -1);
		break;
	case TYPE_UINT:
		*((unsigned int*)value) += step * (increment ? 1 : -1);
		break;

	}
}

void MenuItem::Select() {

}

Menu::Menu(uint8_t nItems, MenuItem** items) :
	nItems(nItems),
	items(items),
	currentItem(0),
	currentButton(NAV_NONE),
	redraw(true)
{
}

void Menu::Init() {
	lcd.begin(LCD_COLS, LCD_ROWS);
	buffer = (char*)malloc(LCD_COLS + 1);
}

uint8_t Menu::GetButton() {
	uint8_t buttons = lcd.readButtons();
	if (buttons & BUTTON_UP) {
		return NAV_UP;
	}
	if (buttons & BUTTON_DOWN) {
		return NAV_DOWN;
	}
	if (buttons & BUTTON_LEFT) {
		return NAV_LEFT;
	}
	if (buttons & BUTTON_RIGHT) {
		return NAV_RIGHT;
	}
	if (buttons & BUTTON_SELECT) {
		return NAV_SELECT;
	}

	return NAV_NONE;
}

void Menu::Draw() {
	if (redraw) {
		lcd.clear();

		buffer[0] = 0;
		items[currentItem]->PrintLabel(buffer, LCD_COLS + 1);
		lcd.setCursor(0, 0);
		lcd.print(buffer);

		buffer[0] = 0;
		items[currentItem]->PrintValue(buffer, LCD_COLS + 1);
		lcd.setCursor(0, 1);
		lcd.print(buffer);

		redraw = false;
	}
}

void Menu::HandleNavigation() {
	byte _button = GetButton();

	if (_button != currentButton) {
		currentButton = _button;

		switch(currentButton) {
		case NAV_DOWN:
			if (currentItem < nItems - 1)
				currentItem++;
			else
				currentItem = 0;
			break;
		case NAV_UP:
			if (currentItem > 0)
				currentItem--;
			else
				currentItem = nItems - 1;
			break;
		case NAV_LEFT:
			items[currentItem]->ChangeValue(false);
			break;
		case NAV_RIGHT:
			items[currentItem]->ChangeValue(true);
			break;
		case NAV_SELECT:
			items[currentItem]->Select();
			break;
		}

		redraw = currentButton != NAV_NONE;
	}
}
