// Do not remove the include below
#include "ArduinoLapse.h"

#include "avr/pgmspace.h"
#include "Menu.h"

#include <SPI.h>
#include <TMC26XStepper.h>

#include "Usb.h"
#include "ptp.h"

#include "MemoryFree.h"

#include "FlexiTimer2.h"

int freeRAM = 0;

unsigned int motorSpeed =  100;
unsigned int motorCurrent = 700;

//                                           <-------------->
prog_char lbl_freeRAM[] PROGMEM =			"Free RAM (bytes)";
prog_char lbl_motorSpeed[] PROGMEM = 		"Mot.Speed (RPM)";
prog_char lbl_motorCurrent[] PROGMEM = 		"Mot.Current (mA)";

MenuItem 	mnu_freeRAM(lbl_freeRAM, TYPE_INT, &freeRAM),
			mnu_motorSpeed(lbl_motorSpeed, TYPE_UINT, &motorSpeed, 10),
			mnu_motorCurrent(lbl_motorCurrent, TYPE_UINT, &motorCurrent, 100);

MenuItem* menu_items[] = {
		&mnu_freeRAM,
		&mnu_motorSpeed,
		&mnu_motorCurrent
};


Menu menu(3, menu_items);
TMC26XStepper tmc26XStepper = TMC26XStepper(200,4,5,6, motorCurrent);

class CamStateHandlers : public PTPStateHandlers
{
      enum CamStates { stInitial, stDisconnected, stConnected };
      CamStates stateConnected;

public:
      CamStateHandlers() : stateConnected(stInitial) {};

      virtual void OnDeviceDisconnectedState(PTP *ptp);
      virtual void OnDeviceInitializedState(PTP *ptp);
} CamStates;

USB      Usb;
PTP      Ptp(&Usb, &CamStates);

void CamStateHandlers::OnDeviceDisconnectedState(PTP *ptp)
{
    if (stateConnected == stConnected || stateConnected == stInitial)
    {
        stateConnected = stDisconnected;
    }
}

void CamStateHandlers::OnDeviceInitializedState(PTP *ptp)
{
    if (stateConnected == stDisconnected || stateConnected == stInitial)
    {
        stateConnected = stConnected;
        ptp->CaptureImage();
    }
}

void on_timer() {
	if (!tmc26XStepper.isMoving()) {
		tmc26XStepper.step(100);
	}
	tmc26XStepper.move();
}

//The setup function is called once at startup of the sketch
void setup()
{
	// Sparkfun USB Host Shield fix
	pinMode(7, OUTPUT);
	digitalWrite(7, HIGH);

	menu.Init();


	tmc26XStepper.setSpreadCycleChopper(2,24,8,6,0);
	tmc26XStepper.setRandomOffTime(0);

	tmc26XStepper.setMicrosteps(4);
	tmc26XStepper.setStallGuardThreshold(4,0);
	tmc26XStepper.start();

	Usb.Init();

	FlexiTimer2::set(1, 1.0 / 10000, on_timer);
	FlexiTimer2::start();
}

// The loop function is called in an endless loop
void loop()
{
	freeRAM = freeMemory();

	menu.HandleNavigation();
	menu.Draw();

	tmc26XStepper.setCurrent(motorCurrent);
	tmc26XStepper.setSpeed(motorSpeed);


	Usb.Task();
}
