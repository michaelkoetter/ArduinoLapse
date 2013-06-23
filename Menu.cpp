#include "Menu.h"
#include "avr/pgmspace.h"

#include "utils.h"

PROGMEM prog_char arrowLeft[8] = {
	B00000,
	B00010,
	B00110,
	B01110,
	B11110,
	B01110,
	B00110,
	B00010
};

PROGMEM prog_char arrowRight[8] = {
	B00000,
	B01000,
	B01100,
	B01110,
	B01111,
	B01110,
	B01100,
	B01000
};

PROGMEM prog_char wrench[8] = {
	B01010,
	B11011,
	B11111,
	B01110,
	B00110,
	B00110,
	B00110,
	B00110
};

PROGMEM prog_char clock[8] = {
	B11100,
	B01010,
	B01001,
	B01001,
	B00101,
	B00010,
	B11100,
	B00000
};


PROGMEM prog_char camera[8] = {
	B11100,
	B00011,
	B11001,
	B11101,
	B11101,
	B11001,
	B00001,
	B11111
};

void MenuItem::Pad(LCD& lcd, byte cols, size_t written) const {
	while (written < cols) {
		written += lcd.print(F(" "));
	}
}

LabelMenuItem::LabelMenuItem(const __FlashStringHelper* label)
	: m_label(label)
{
}

void LabelMenuItem::Render(LCD& lcd, byte cols, byte rows) const {
	lcd.setCursor(0, 0);
	RenderLabel(lcd, cols, rows);
}

void LabelMenuItem::RenderLabel(LCD& lcd, byte cols, byte rows) const {
	Pad(lcd, cols, lcd.print(m_label));
}

/**
 * Render user friendly time representation
 */
void TimeConfigMenuItem::RenderValue(LCD& lcd, byte cols, byte rows) const {
	size_t size = 0;
	int remain = m_value->Get();

	// hours
	int current = remain / 3600;
	remain -= current * 3600;
	size += lcd.print(current);
	size += lcd.print(F("h "));

	// minutes
	current = remain / 60; //
	remain -= current * 60;
	size += lcd.print(current);
	size += lcd.print(F("' "));

	// seconds remain
	size += lcd.print(remain);
	size += lcd.print(F("\""));

	Pad(lcd, cols, size);
}

void BacklightConfigMenuItem::RenderValue(LCD& lcd, byte cols, byte rows) const {
	switch(m_value->Get()) {
	case OFF:
		Pad(lcd, cols, lcd.print(F("Off")));
		break;
	case RED:
		Pad(lcd, cols, lcd.print(F("Red")));
		break;
	case YELLOW:
		Pad(lcd, cols, lcd.print(F("Yellow")));
		break;
	case GREEN:
		Pad(lcd, cols, lcd.print(F("Green")));
		break;
	case TEAL:
		Pad(lcd, cols, lcd.print(F("Teal")));
		break;
	case BLUE:
		Pad(lcd, cols, lcd.print(F("Blue")));
		break;
	case VIOLET:
		Pad(lcd, cols, lcd.print(F("Violet")));
		break;
	case WHITE:
		Pad(lcd, cols, lcd.print(F("White")));
		break;
	default:
		Pad(lcd, cols, lcd.print(F("Unknown")));
	}
}

void TriggerModeConfigMenuItem::RenderValue(LCD& lcd, byte cols, byte rows) const {
	switch(m_value->Get()) {
	case TRIGGER_CABLE:
		Pad(lcd, cols, lcd.print(F("Cbl. Release")));
		break;
	case TRIGGER_PTP:
		Pad(lcd, cols, lcd.print(F("USB/PTP")));
		break;
	default:
		Pad(lcd, cols, lcd.print(F("Unknown")));
	}
}

void MicrostepsConfigMenuItem::RenderValue(LCD& lcd, byte cols, byte rows) const {
	Pad(lcd, cols, lcd.print(ipow(2, m_value->Get())));
}

Menu::Menu(LCD* lcd,
		const byte cols,
		const byte rows)
	: m_lcd(lcd), m_rows(rows), m_cols(cols),
	  m_head(NULL), m_tail(NULL), m_current(NULL), m_idle(NULL),
	  m_currentButtons(0), m_fastForwardTimeout(1000), m_idleTimeout(5000)
{
}

void Menu::Init() {
	CreateChar(ARROW_LEFT, arrowLeft);
	CreateChar(ARROW_RIGHT, arrowRight);
	CreateChar(WRENCH, wrench);
	CreateChar(CLOCK, clock);
	CreateChar(CAMERA, camera);

	m_lcd->clear();
}

void Menu::CreateChar(byte code, PGM_P character) {
	byte* buffer = (byte*)malloc(8);
	memcpy_P(buffer, character,  8);
	m_lcd->createChar(code, buffer);
	free(buffer);
}

void Menu::Render() {
	HandleNavigation();
	unsigned long now = millis();
	if (now - m_buttonsChanged > m_idleTimeout
			&& m_currentButtons == 0
			&& m_idle != NULL) {
		m_current = NULL;
		m_idle->Render(*m_lcd, m_cols, m_rows);
	} else if (m_current != NULL) {
		m_current->Render(*m_lcd, m_cols, m_rows);
	}
}

void Menu::AddMenuItem(MenuItem* item) {
	if (m_head == NULL) {
		// assume m_tail == NULL && m_current == NULL
		// initialize with first menu item
		m_head = m_tail = m_current = item;
	} else {
		// assume m_tail != NULL
		// append menu item
		m_tail->m_next = item;
		m_head->m_prev = item;

		item->m_prev = m_tail;
		item->m_next = m_head;
		m_tail = item;
	}
}

void Menu::SetIdleScreen(MenuItem* item) {
	m_idle = item;
}

void Menu::HandleNavigation() {
	unsigned long now = millis();

	byte buttons = m_lcd->readButtons();
	if (buttons != m_currentButtons) {
		m_buttonsChanged = now;
	}

	if (now - m_buttonsChanged > 1000 || buttons != m_currentButtons) {
		m_currentButtons = buttons;
		if (buttons & BUTTON_UP) {
			if (m_current != NULL && m_current->m_prev != NULL) {
				m_current = m_current->m_prev;
			}
		} else if (buttons & BUTTON_DOWN) {
			if (m_current != NULL && m_current->m_next != NULL) {
				m_current = m_current->m_next;
			}
		}

		if (m_current == NULL) {
			m_current = m_head;
		} else {
			m_current->HandleButtons(buttons);
		}
	}
}
