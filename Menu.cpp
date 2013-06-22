#include "Menu.h"

LabelMenuItem::LabelMenuItem(const __FlashStringHelper* label)
	: m_label(label)
{
}

void LabelMenuItem::Render(LCD& lcd) const {
	lcd.setCursor(0, 0);
	lcd.print(m_label);
}

LabelValueMenuItem::LabelValueMenuItem(const __FlashStringHelper* label, const MenuValueBase* value)
	: LabelMenuItem(label), m_value(value)
{
}

void LabelValueMenuItem::Render(LCD& lcd) const {
	LabelMenuItem::Render(lcd);
	lcd.setCursor(1, 0);
	lcd.print(*m_value);
}


Menu::Menu(LCD* lcd,
		const byte cols,
		const byte rows,
		const MenuItem** items,
		unsigned int nItems)
	: m_lcd(lcd), m_rows(rows), m_cols(cols),
	  m_items(items), m_nItems(nItems), m_currentItem(0)
{
}

void Menu::Init() {
	m_lcd->begin(m_cols, m_rows);
	m_lcd->clear();

	m_lcd->setBacklight(RED);
}

void Menu::Render() {

}
