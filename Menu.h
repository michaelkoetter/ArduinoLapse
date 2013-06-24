#ifndef MENU_H_
#define MENU_H_

#include "Arduino.h"
#include "avr/pgmspace.h"

#include "Print.h"

#include "LiquidTWI2.h"

#include "Config.h"

#define LCD LiquidTWI2

#define ARROW_RIGHT (char)0x0
#define ARROW_LEFT  (char)0x1
#define WRENCH		(char)0x2
#define CLOCK		(char)0x3
#define CAMERA		(char)0x4

class MenuItem {
friend class Menu;

public:

	virtual void Render(LCD& lcd, byte cols, byte rows) const = 0;
	virtual void HandleButtons(byte buttons) {};

protected:
	void Pad(LCD& lcd, byte cols, size_t written) const;
private:
	MenuItem	*m_next, *m_prev;
};

class ConfigMenuItem : public MenuItem {
public:
	ConfigMenuItem(const __FlashStringHelper* label,
			ConfigValue* value,
			int step = 1)
		: m_label(label), m_value(value), m_step(step) {};

	virtual void Render(LCD& lcd, byte cols, byte rows) const;

	virtual void HandleButtons(byte buttons) {
		if (buttons & BUTTON_LEFT) {
			m_value->Modify(-m_step);
		} else if (buttons & BUTTON_RIGHT) {
			m_value->Modify(m_step);
		}
	}

protected:
	const __FlashStringHelper		*m_label;
	ConfigValue						*m_value;
	int								m_step;
};

class Menu {
public:
	Menu(LCD* lcd,
		const byte cols,
		const byte rows);

	void 					Init();
	void					Render();

	void					AddMenuItem(MenuItem* item);
	void					SetIdleScreen(MenuItem* item);

private:
	LCD						*m_lcd;
	const byte				m_rows;
	const byte				m_cols;

	MenuItem				*m_head, *m_tail, *m_current, *m_idle;

	byte					m_currentButtons;
	unsigned long			m_buttonsChanged;
	unsigned int			m_fastForwardTimeout, m_idleTimeout;

	void					HandleNavigation();
	void					CreateChar(byte code, PGM_P character);
};

#endif /* MENU_H_ */
