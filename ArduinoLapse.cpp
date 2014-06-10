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

ConfigValue motorSpeed(250, 10, 400, NULL, on_change_motor_speed);
ConfigValue motorMicrosteps(2, 0, 8, PrintMicrosteps, on_change_motor_microsteps);
ConfigValue motorCurrent(1000, 100, 1200, PrintCurrent);
ConfigValue motorIdleCurrent(500, 0, 1200, PrintCurrent);
ConfigValue backlight(RED, RED, WHITE, PrintBacklightColor, on_change_backlight);

ConfigValue interval(2, 1, LONG_MAX - 1, PrintTime);
ConfigValue stabilize(1, 0, 10, PrintTime);
ConfigValue numShots(300, 0, LONG_MAX - 1);

// full slider length movement in "full step" mode
// needs to be multiplied for microstepping
ConfigValue movement(135000, 0, LONG_MAX - 1);


TMC26XStepper	stepper(200, PIN_TOS100_CS, PIN_TOS100_DIR,
		PIN_TOS100_STEP, motorCurrent.Get());

USB				usb;

Sequence		sequence(&usb, &stepper,
		&numShots, &movement, &motorMicrosteps, &interval, &stabilize);

LCD				lcd(MCP23017_ADDRESS);
Menu			menu(&lcd, LCD_COLS, LCD_ROWS);

unsigned long	slow_timing = 0;
unsigned int 	current = 0;

// 50kHz timer
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

void on_change_motor_speed(long& value) {
	stepper.setSpeed((int)value);
}

void on_change_motor_microsteps(long& value) {
	stepper.setMicrosteps(ipow(2, (int)value));
}

void on_change_backlight(long& value) {
	lcd.setBacklight((int)value);
}

//The setup function is called once at startup of the sketch
void setup()
{
	// Sparkfun USB Host Shield fix
	pinMode(PIN_USB_FIX, OUTPUT);
	digitalWrite(PIN_USB_FIX, HIGH);

	usb.Init();

	stepper.start();
	stepper.setMicrosteps(ipow(2, motorMicrosteps.Get()));
	stepper.setSpeed(motorSpeed.Get());

	menu.AddMenuItem(new ActionMenuItem(F("\4 Start Sequence"),
			F("  [Select]"), on_start_sequence, FLAG_IDLE));
	menu.AddMenuItem(new ActionMenuItem(F("\4 Stop Sequence"),
			F("  [Select]"), on_stop_sequence, FLAG_RUNNING));

	// settings
	menu.AddMenuItem(new ConfigMenuItem(F("\5 Shots"), &numShots, 10, FLAG_IDLE));
	menu.AddMenuItem(new ConfigMenuItem(F("\5 Sl.Movement"), &movement, 500, FLAG_IDLE));
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

	// 50kHz timer
	FlexiTimer2::set(1, 1.0 / 50000, on_timer);
	FlexiTimer2::start();
}

// executed approximately every 100ms
void slow_loop() {
	freeRam.Set(freeMemory());
	menu.Render();
}

// The loop function is called in an endless loop
void loop()
{
	unsigned int desired_current = 0;
	if (!stepper.isMoving()) {
		desired_current = (int)motorIdleCurrent.Get();
	} else {
		desired_current = (int)motorCurrent.Get();
	}

	if (current != desired_current) {
		stepper.setCurrent(desired_current);
		current = desired_current;
	}

	unsigned long now = millis();
	if (now - slow_timing > 100) {
		slow_timing = now;
		slow_loop();
	}

	usb.Task();
	sequence.Loop();
}

