// Do not remove the include below
#include "ArduinoLapse.h"

#include "avr/pgmspace.h"

#include "Config.h"
#include "Menu.h"
#include "Sequence.h"

#include <SPI.h>
#include <TMC26XStepper.h>

#include "Usb.h"
#include "ptp.h"

#include "MemoryFree.h"

#include "FlexiTimer2.h"

#define LCD_COLS 16
#define LCD_ROWS 2

ConfigValue<int> freeRam(0);
ConfigValue<int> motorSpeed(100, 10, 400);
ConfigValue<int> motorCurrent(1000, 100, 1200);
ConfigValue<int> motorIdleCurrent(200, 0, 1200);
ConfigValue<int> backlight(RED, RED, WHITE);
ConfigValue<int> interval(10, 1, 18000);
ConfigValue<int> stabilize(2, 0, 10);


TMC26XStepper	stepper(200,4,5,6, motorCurrent.Get());
USB				usb;
Sequence		sequence(&usb, &stepper);
LCD				lcd(MCP23017_ADDRESS);
Menu			menu(&lcd, LCD_COLS, LCD_ROWS);

void on_timer() {
	if (!stepper.isMoving()) {
		stepper.step(100);
	}
	stepper.move();
}

//The setup function is called once at startup of the sketch
void setup()
{
	// Sparkfun USB Host Shield fix
	pinMode(7, OUTPUT);
	digitalWrite(7, HIGH);

	usb.Init();

	stepper.setSpreadCycleChopper(2,24,8,6,0);
	stepper.setRandomOffTime(0);
	stepper.setMicrosteps(4);
	stepper.setStallGuardThreshold(4,0);
	stepper.start();

	FlexiTimer2::set(1, 1.0 / 10000, on_timer);
	FlexiTimer2::start();

	menu.SetIdleScreen(new LabelMenuItem(F("Idle")));

	menu.AddMenuItem(new TimeConfigMenuItem		(F("\3 Interval"), &interval));
	menu.AddMenuItem(new TimeConfigMenuItem		(F("\4 Stabilize"), &stabilize));
	menu.AddMenuItem(new ConfigMenuItem<int>	(F("\2 Motor Speed"), &motorSpeed, F(" RPM")));
	menu.AddMenuItem(new ConfigMenuItem<int>	(F("\2 Motor Current"), &motorCurrent, F(" mA")));
	menu.AddMenuItem(new ConfigMenuItem<int>	(F("\2 M.Idle Current"), &motorIdleCurrent, F(" mA")));
	menu.AddMenuItem(new BacklightConfigMenuItem	(F("\2 Backlight"), &backlight));
	menu.AddMenuItem(new ConfigMenuItem<int>	(F("i Free RAM"), &freeRam));


	lcd.setMCPType(LTI_TYPE_MCP23017);
	lcd.begin(LCD_COLS, LCD_ROWS);
	lcd.setBacklight(backlight.Get());

	lcd.clear();
	lcd.print(F("  ArduinoLapse"));
	lcd.setCursor(0, 1);
	lcd.print(F("     v0.1"));
	delay(2000);

	menu.Init();
}

// The loop function is called in an endless loop
void loop()
{
	freeRam.Set(freeMemory());

	stepper.setCurrent(motorCurrent.Get());
	stepper.setSpeed(motorSpeed.Get());

	usb.Task();
	sequence.Loop();

	menu.Render();

	lcd.setBacklight(backlight.Get());
}
