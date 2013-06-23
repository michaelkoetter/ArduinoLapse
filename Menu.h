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

class LabelMenuItem : public MenuItem {
public:
	LabelMenuItem(const __FlashStringHelper* label);
	virtual void Render(LCD& lcd, byte cols, byte rows) const;

protected:
	virtual void RenderLabel(LCD& lcd, byte cols, byte rows) const;

private:
	const __FlashStringHelper	*m_label;
};

template<class T>
class ConfigMenuItem : public LabelMenuItem {
public:
	ConfigMenuItem(const __FlashStringHelper* label,
			ConfigValue<T>* value,
			const __FlashStringHelper* unit = NULL)
		: LabelMenuItem(label), m_value(value), m_unit(unit) {};

	virtual void Render(LCD& lcd, byte cols, byte rows) const {
		LabelMenuItem::Render(lcd, cols, rows);
		lcd.setCursor(2, 1);
		RenderValue(lcd, cols - 4, rows);
		RenderNav(lcd, cols, 1);
	}

	virtual void HandleButtons(byte buttons) {
		if (buttons & BUTTON_LEFT) {
			m_value->Modify(-1);
		} else if (buttons & BUTTON_RIGHT) {
			m_value->Modify(1);
		}
	}

protected:
	virtual void RenderNav(LCD& lcd, byte cols, byte row) const {
		T value = m_value->Get();
		lcd.setCursor(0, row);
		if (value > m_value->GetMin()) {
			lcd.print(ARROW_LEFT);
		} else {
			lcd.print(F(" "));
		}

		lcd.setCursor(cols - 1, row);
		if (value < m_value->GetMax()) {
			lcd.print(ARROW_RIGHT);
		} else {
			lcd.print(F(" "));
		}
	}

	virtual void RenderValue(LCD& lcd, byte cols, byte rows) const {
		size_t size = lcd.print(m_value->Get());
		if (m_unit != NULL) {
			size += lcd.print(m_unit);
		}
		Pad(lcd, cols, size);
	}

	const __FlashStringHelper		*m_unit;
	ConfigValue<T>			*m_value;
};

class TimeConfigMenuItem : public ConfigMenuItem<int> {
public:
	TimeConfigMenuItem(const __FlashStringHelper* label, ConfigValue<int>* value)
			: ConfigMenuItem<int>(label, value) {};
protected:
	virtual void RenderValue(LCD& lcd, byte cols, byte rows) const;
};

class BacklightConfigMenuItem : public ConfigMenuItem<int> {
public:
	BacklightConfigMenuItem(const __FlashStringHelper* label, ConfigValue<int>* value)
			: ConfigMenuItem<int>(label, value) {};
protected:
	virtual void RenderValue(LCD& lcd, byte cols, byte rows) const;
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
