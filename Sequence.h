#ifndef CAMERA_H_
#define CAMERA_H_

#include "Arduino.h"
#include "Usb.h"
#include "ptp.h"

#include "TMC26XStepper.h"

#include "Config.h"

#define CAM_CONNECTED B1000000

class Sequence : public PTPStateHandlers {
	enum CamStates { stInitial, stDisconnected, stConnected };
	PTP			ptp;
	CamStates 	state;

	TMC26XStepper	*stepper;

	long			m_shotsRemaining;
    unsigned long	m_nextTrigger;
    unsigned long 	m_nextMove;
    unsigned long	m_startTime;
    long			m_stepsPerMovement;
    long			m_position;

    ConfigValue		*m_interval;
    ConfigValue		*m_stabilize;
    ConfigValue		*m_shots;
    ConfigValue		*m_movement;
    ConfigValue		*m_microsteps;

	void Trigger();

public:

    Sequence(USB *usb, TMC26XStepper *stepper,
    		ConfigValue *shots,
    		ConfigValue *movement,
    		ConfigValue *microsteps,
    		ConfigValue *interval,
    		ConfigValue *stabilize);

	void Loop();

	void Start();
	void Stop() { m_shotsRemaining = 0; };

	bool IsRunning() { return m_shotsRemaining > 0; }

	int GetShotsRemaining() { return m_shotsRemaining; }
	int GetShotsTotal() { return m_shots->Get(); }
	int GetShotsFired() { return m_shots->Get() - m_shotsRemaining; }
	long GetSecondsRemaining() {
		unsigned long now = millis();
		unsigned long elapsed = (now - m_startTime) / 1000;
		return m_interval->Get() * GetShotsTotal() - elapsed;
	}

	// PTPStateHandlers
    virtual void OnDeviceDisconnectedState(PTP *ptp);
    virtual void OnDeviceInitializedState(PTP *ptp);
};

#endif /* CAMERA_H_ */
