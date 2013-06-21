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
