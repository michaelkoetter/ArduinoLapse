// Do not remove the include below
#include "ArduinoLapse.h"

#include "avr/pgmspace.h"

#include "Menu.h"
#include "Sequence.h"

#include <SPI.h>
#include <TMC26XStepper.h>

#include "Usb.h"
#include "ptp.h"

#include "MemoryFree.h"

#include "FlexiTimer2.h"

int freeRAM = 0;

unsigned int motorSpeed =  100;
unsigned int motorCurrent = 500;



TMC26XStepper	stepper(200,4,5,6, motorCurrent);
USB				usb;
Sequence		sequence(&usb, &stepper);

//                                           <-------------->
prog_char lbl_interval[] PROGMEM =			"Interval (s)";
prog_char lbl_motorSpeed[] PROGMEM = 		"Mot.Speed (RPM)";
prog_char lbl_motorCurrent[] PROGMEM = 		"Mot.Current (mA)";
prog_char lbl_freeRAM[] PROGMEM =			"Free RAM (bytes)";

MenuItem 	mnu_interval(lbl_interval, TYPE_UINT, &sequence.interval),
			mnu_motorSpeed(lbl_motorSpeed, TYPE_UINT, &motorSpeed, 10),
			mnu_motorCurrent(lbl_motorCurrent, TYPE_UINT, &motorCurrent, 100),
			mnu_freeRAM(lbl_freeRAM, TYPE_INT, &freeRAM);

MenuItem* menu_items[] = {
		&mnu_interval,
		&mnu_motorSpeed,
		&mnu_motorCurrent,
		&mnu_freeRAM
};

Menu 			menu(3, menu_items);

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

	menu.Init();
}

// The loop function is called in an endless loop
void loop()
{
	freeRAM = freeMemory();

	stepper.setCurrent(motorCurrent);
	stepper.setSpeed(motorSpeed);

	usb.Task();

	sequence.Loop();

	menu.HandleNavigation();
	menu.Draw();
}
