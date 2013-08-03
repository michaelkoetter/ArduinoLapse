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

	int				m_shotsRemaining;
    unsigned long	m_nextTrigger;
    unsigned long 	m_nextMove;
    int				m_stepsPerMovement;
    int				m_position;

    ConfigValue		*m_interval;
    ConfigValue		*m_stabilize;
    ConfigValue		*m_shots;
    ConfigValue		*m_movement;


	void Trigger();

public:



    Sequence(USB *usb, TMC26XStepper *stepper,
    		ConfigValue *shots,
    		ConfigValue *movement,
    		ConfigValue *interval,
    		ConfigValue *stabilize);

	void Loop();

	void Start();
	void Stop() { m_shotsRemaining = 0; };

	bool IsRunning() { return m_shotsRemaining > 0; }

	int GetShotsRemaining() { return m_shotsRemaining; }
	int GetShotsTotal() { return m_shots->Get(); }
	int GetShotsFired() { return m_shots->Get() - m_shotsRemaining; }
	int GetSecondsRemaining() { return m_interval->Get() * m_shotsRemaining; }

	// PTPStateHandlers
    virtual void OnDeviceDisconnectedState(PTP *ptp);
    virtual void OnDeviceInitializedState(PTP *ptp);
};

#endif /* CAMERA_H_ */
