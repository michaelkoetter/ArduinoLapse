/*
 * Camera.h
 *
 *  Created on: 10.06.2013
 *      Author: michael
 */

#ifndef CAMERA_H_
#define CAMERA_H_

#include "Arduino.h"
#include "Usb.h"
#include "ptp.h"

#include "TMC26XStepper.h"

#define CAM_CONNECTED B1000000

class Sequence : public PTPStateHandlers {
	enum CamStates { stInitial, stDisconnected, stConnected };
	PTP			ptp;
	CamStates 	state;

	TMC26XStepper	*stepper;

	unsigned long	lastTrigger;

	void Trigger();

public:
    // interval in seconds
    unsigned int	interval;

    // number of shots
    unsigned int	shots;

    Sequence(USB *usb, TMC26XStepper *stepper);

	void Loop();

	// PTPStateHandlers
    virtual void OnDeviceDisconnectedState(PTP *ptp);
    virtual void OnDeviceInitializedState(PTP *ptp);
};

#endif /* CAMERA_H_ */
