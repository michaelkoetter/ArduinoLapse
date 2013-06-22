#ifndef MENU_H_
#define MENU_H_

#include "Arduino.h"
#include "avr/pgmspace.h"

#include "Print.h"

#include "LiquidTWI2.h"

#define LCD LiquidTWI2

class MenuValueBase : public Printable {
public:
	virtual void Modify(int amount) const = 0;
};

template<class T>
class MenuValue : public MenuValueBase {
public:
	MenuValue(T* value, T steps = 1)
		: m_value(value), m_steps(steps) {}

	virtual size_t printTo(Print& p) const {
		p.print(*m_value);
	}

	virtual void Modify(int amount) const {
		(*m_value) += (amount * m_steps);
	}

private:
	T*	m_value;
	T 	m_steps;
};


class MenuItem {
public:
	virtual void Render(LCD& lcd) const = 0;
};

class LabelMenuItem : public MenuItem {
public:
	LabelMenuItem(const __FlashStringHelper* label);
	virtual void Render(LCD& lcd) const;

private:
	const __FlashStringHelper*	m_label;
};

class LabelValueMenuItem : public LabelMenuItem {
public:
	LabelValueMenuItem(const __FlashStringHelper* label, const MenuValueBase* value);
	virtual void Render(LCD& lcd) const;

private:
	const MenuValueBase*	m_value;
};


class Menu {
public:
	Menu(LCD* lcd,
		const byte cols,
		const byte rows,
		const MenuItem** items,
		unsigned int nItems);

	void 					Init();
	void					Render();

private:
	const MenuItem**		m_items;
	const unsigned int	m_nItems;

	LCD*					m_lcd;
	const byte				m_rows;
	const byte				m_cols;

	unsigned int			m_currentItem;
};

#endif /* MENU_H_ */
