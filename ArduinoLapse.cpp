// Do not remove the include below
#include "ArduinoLapse.h"

#include "avr/pgmspace.h"
#include "limits.h"

#include "Config.h"
#include "Menu.h"
#include "Sequence.h"

#include <SPI.h>
#include <TMC26XStepper.h>

#include "Usb.h"
#include "ptp.h"

#include "MemoryFree.h"
#include "FlexiTimer2.h"

#include "utils.h"

#define LCD_COLS 16
#define LCD_ROWS 2

#define FLAG_IDLE B1000000
#define FLAG_RUNNING B0100000

// I/O Pins
#define PIN_TOS100_CS 4
#define PIN_TOS100_DIR 5
#define PIN_TOS100_STEP 6

#define PIN_USB_FIX 7

ConfigValue freeRam(0);

ConfigValue triggerMode(1, 0, 1, PrintTriggerMode);
ConfigValue motorSpeed(200, 10, 400);
ConfigValue motorMicrosteps(2, 0, 8, PrintMicrosteps);
ConfigValue motorCurrent(800, 100, 1200, PrintCurrent);
ConfigValue motorIdleCurrent(400, 0, 1200, PrintCurrent);
ConfigValue backlight(RED, RED, WHITE, PrintBacklightColor);

ConfigValue interval(2, 1, LONG_MAX - 1, PrintTime);
ConfigValue stabilize(1, 0, 10, PrintTime);
ConfigValue numShots(300, 0, LONG_MAX - 1);

// full slider length movement in "full step" mode
// needs to be multiplied for microstepping
ConfigValue movement(142500, 0, LONG_MAX - 1);


TMC26XStepper	stepper(200, PIN_TOS100_CS, PIN_TOS100_DIR,
		PIN_TOS100_STEP, motorCurrent.Get());

USB				usb;

Sequence		sequence(&usb, &stepper,
		&numShots, &movement, &motorMicrosteps, &interval, &stabilize);

LCD				lcd(MCP23017_ADDRESS);
Menu			menu(&lcd, LCD_COLS, LCD_ROWS);

void on_timer() {
	stepper.move();
}

void on_start_sequence(byte buttons) {
	if (buttons & BUTTON_SELECT) {
		menu.SetMask(FLAG_RUNNING);
		sequence.Start();
	}
}

void on_stop_sequence(byte buttons) {
	if (buttons & BUTTON_SELECT) {
		menu.SetMask(FLAG_IDLE);
		sequence.Stop();
	}
}

void on_move_slider(byte buttons) {
	if (!stepper.isMoving()) {
		long move = (movement.Get() / 1000)
				* (ipow(2, (int)motorMicrosteps.Get()));

		if (buttons & BUTTON_LEFT) {
			stepper.step(-1 * move);
		} else if (buttons & BUTTON_RIGHT) {
			stepper.step(move);
		}
	}
}

//The setup function is called once at startup of the sketch
void setup()
{
	// Sparkfun USB Host Shield fix
	pinMode(PIN_USB_FIX, OUTPUT);
	digitalWrite(PIN_USB_FIX, HIGH);

	usb.Init();

	stepper.start();


	menu.AddMenuItem(new ActionMenuItem(F("\4 Start Sequence"),
			F("  [Press Select]"), on_start_sequence, FLAG_IDLE));
	menu.AddMenuItem(new ActionMenuItem(F("\4 Stop Sequence"),
			F("  [Press Select]"), on_stop_sequence, FLAG_RUNNING));

	// settings
	menu.AddMenuItem(new ConfigMenuItem(F("\5 Shots"), &numShots, 10, FLAG_IDLE));
	menu.AddMenuItem(new ConfigMenuItem(F("\5 Movement"), &movement, 500, FLAG_IDLE));
	menu.AddMenuItem(new ConfigMenuItem(F("\4 Interval"), &interval, 1, FLAG_IDLE));
	menu.AddMenuItem(new ConfigMenuItem(F("\4 Stabilize"), &stabilize, 1, FLAG_IDLE));

	// config
	menu.AddMenuItem(new ConfigMenuItem(F("\3 Trigger Mode"), &triggerMode));
	menu.AddMenuItem(new ConfigMenuItem(F("\3 Mot.Speed"), &motorSpeed, 10));
	menu.AddMenuItem(new ConfigMenuItem(F("\3 Mot.Microsteps"), &motorMicrosteps));
	menu.AddMenuItem(new ConfigMenuItem(F("\3 Mot.Current"), &motorCurrent, 50));
	menu.AddMenuItem(new ConfigMenuItem(F("\3 M.Idle Current"), &motorIdleCurrent, 50));
	menu.AddMenuItem(new ConfigMenuItem(F("\3 Backlight"), &backlight));
	menu.AddMenuItem(new ActionMenuItem(F("\3 Move Slider"),
			F("  [\2\1 Move]"), on_move_slider, FLAG_IDLE));

	// info
	menu.AddMenuItem(new ConfigMenuItem(F("i Free RAM"), &freeRam));

	menu.SetIdleScreen(new InfoIdleScreen(&sequence));

	lcd.setMCPType(LTI_TYPE_MCP23017);
	lcd.begin(LCD_COLS, LCD_ROWS);
	lcd.setBacklight(backlight.Get());

	lcd.clear();
	lcd.print(F("  ArduinoLapse"));
	lcd.setCursor(0, 1);
	lcd.print(F("     v0.1"));
	delay(2000);

	menu.SetMask(FLAG_IDLE);
	menu.Init();

	FlexiTimer2::set(1, 1.0 / 20000, on_timer);
	FlexiTimer2::start();
}

// The loop function is called in an endless loop
void loop()
{
	freeRam.Set(freeMemory());

	if (!stepper.isMoving()) {
		stepper.setCurrent(motorIdleCurrent.Get());
	} else {
		stepper.setCurrent(motorCurrent.Get());
	}
	stepper.setMicrosteps(ipow(2, motorMicrosteps.Get()));
	stepper.setSpeed(motorSpeed.Get());

	usb.Task();
	sequence.Loop();

	menu.Render();

	lcd.setBacklight(backlight.Get());
}
