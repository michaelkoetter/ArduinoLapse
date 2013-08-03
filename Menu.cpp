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

void ConfigMenuItem::Render(LCD& lcd, byte cols, byte rows) const {
	size_t size = 0;
	int value = m_value->Get();

	// render label
	lcd.setCursor(0, 0);
	size = lcd.print(m_label);
	Pad(lcd, cols, size);

	// render value
	lcd.setCursor(2, 1);
	size = m_value->PrintValue(lcd);
	Pad(lcd, cols - 4, size);

	// render navigation
	lcd.setCursor(0, 1);
	if (value > m_value->GetMin()) {
		lcd.print(ARROW_LEFT);
	} else {
		lcd.print(F(" "));
	}
	lcd.print(F(" "));

	lcd.setCursor(cols - 2, 1);
	lcd.print(F(" "));
	if (value < m_value->GetMax()) {
		lcd.print(ARROW_RIGHT);
	} else {
		lcd.print(F(" "));
	}
}

void ActionMenuItem::Render(LCD& lcd, byte cols, byte rows) const {
	lcd.setCursor(0, 0);
	Pad(lcd, cols, lcd.print(m_label));

	lcd.setCursor(0, 1);
	Pad(lcd, cols, lcd.print(m_prompt));
}

void ActionMenuItem::HandleButtons(byte buttons) {
	if (buttons & BUTTON_SELECT) {
		if (m_action != NULL) {
			(*m_action)();
		}
	}
}

InfoIdleScreen::InfoIdleScreen(Sequence *sequence)
	: m_sequence(sequence) {
}

void InfoIdleScreen::Render(LCD& lcd, byte cols, byte rows) const {
	lcd.setCursor(0, 0);
	if (m_sequence->IsRunning()) {
		int _chars = 0;
		_chars += lcd.print(F("Shot "));
		_chars += lcd.print(m_sequence->GetShotsFired());
		_chars += lcd.print(F("/"));
		_chars += lcd.print(m_sequence->GetShotsTotal());
		Pad(lcd, cols, _chars);

		_chars = 0;
		lcd.setCursor(0, 1);
		_chars += lcd.print(F("Rem. "));
		int _secondsRemaining = m_sequence->GetSecondsRemaining();
		_chars += PrintTime(_secondsRemaining, lcd);
		Pad(lcd, cols, _chars);
	} else {
		Pad(lcd, cols, lcd.print(F("Idle")));
		lcd.setCursor(0, 1);
		Pad(lcd, cols, 0);
	}
}

Menu::Menu(LCD* lcd,
		const byte cols,
		const byte rows)
	: m_lcd(lcd), m_rows(rows), m_cols(cols),
	  m_head(NULL), m_tail(NULL), m_current(NULL), m_idle(NULL),
	  m_currentButtons(-1), m_fastForwardTimeout(1000), m_idleTimeout(5000),
	  m_mask(0xff)
{
}

void Menu::Init() {
	CreateChar(ARROW_LEFT, arrowLeft);
	CreateChar(ARROW_RIGHT, arrowRight);
	CreateChar(WRENCH, wrench);
	CreateChar(CLOCK, clock);
	CreateChar(CAMERA, camera);

	m_lcd->clear();
	GotoFirstItem();
}

void Menu::CreateChar(byte code, PGM_P character) {
	byte* buffer = (byte*)malloc(8);
	memcpy_P(buffer, character,  8);
	m_lcd->createChar(code, buffer);
	free(buffer);
}

void Menu::Render() {
	unsigned long now = millis();
	if (now - m_buttonsChanged > m_idleTimeout
			&& m_currentButtons == 0
			&& m_idle != NULL
			&& !m_idleActive) {
		ActivateIdleScreen();
	}

	HandleNavigation();

	if (m_idleActive && m_idle != NULL) {
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

void Menu::ActivateIdleScreen(bool active) {
	if (active) {
		m_current = NULL;
	}

	m_idleActive = active;
}

void Menu::SetMask(byte mask) {
	m_mask = mask;
	GotoFirstItem();
}

void Menu::HandleNavigation() {
	unsigned long now = millis();

	byte buttons = m_lcd->readButtons();

	if (now - m_buttonsChanged > 1000 || buttons != m_currentButtons) {
		if (buttons != m_currentButtons) {
			m_buttonsChanged = now;
		}

		m_currentButtons = buttons;

		if (buttons != 0) {
			if (buttons & BUTTON_UP) {
				do {
					if (m_current != NULL) {
						m_current = m_current->m_prev;
					}
				} while (!(m_current->m_flag & m_mask));
			} else if (buttons & BUTTON_DOWN) {
				do {
					if (m_current != NULL) {
						m_current = m_current->m_next;
					}
				} while (!(m_current->m_flag & m_mask));
			}


			if (m_current == NULL) {
				GotoFirstItem();
			} else {
				m_current->HandleButtons(buttons);
			}

			ActivateIdleScreen(false);
		}

	}
}

void Menu::GotoFirstItem() {
	m_current = m_head;
	while (m_current != NULL && !(m_current->m_flag & m_mask)) {
		m_current = m_current->m_next;
	}
}
