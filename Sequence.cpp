/*
 * Camera.cpp
 *
 *  Created on: 10.06.2013
 *      Author: michael
 */

#include "Sequence.h"

Sequence::Sequence(USB *usb, TMC26XStepper *stepper)
	: ptp(usb, this),
	  stepper(stepper),
	  state(stInitial),
	  interval(10),
	  shots(1000),
	  lastTrigger(millis())
{
}

void Sequence::Trigger() {
	if (state == stConnected) {
		ptp.CaptureImage();
	}
}

void Sequence::Loop() {
	unsigned long _time = millis();
	if (_time - lastTrigger > (interval * 1000)) {
		lastTrigger = _time;
		Trigger();
	}
}

void Sequence::OnDeviceInitializedState(PTP *ptp) {
	state = stConnected;
}

void Sequence::OnDeviceDisconnectedState(PTP *ptp) {
	state = stDisconnected;
}
