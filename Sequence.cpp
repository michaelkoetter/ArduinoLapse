#include "Sequence.h"

#include "utils.h"

Sequence::Sequence(USB *usb, TMC26XStepper *stepper,
		ConfigValue *shots,
		ConfigValue *movement,
		ConfigValue *microsteps,
		ConfigValue *interval,
		ConfigValue *stabilize)
	: ptp(usb, this),
	  stepper(stepper),
	  state(stInitial),
	  m_shots(shots),
	  m_movement(movement),
	  m_microsteps(microsteps),
	  m_interval(interval),
	  m_stabilize(stabilize),
	  m_nextMove(0),
	  m_nextTrigger(0),
	  m_shotsRemaining(0),
	  m_position(0),
	  m_stepsPerMovement(0),
	  m_startTime(0)
{
}


void Sequence::Trigger() {
	if (state == stConnected) {
		ptp.CaptureImage();
	}
}

void Sequence::Start() {
	unsigned long now = millis();
	m_startTime = now;
	m_shotsRemaining = m_shots->Get();

	m_nextMove = now + (m_interval->Get() * 1000);
	m_nextTrigger = now + (m_stabilize->Get() * 1000);

	m_stepsPerMovement = (m_movement->Get() / m_shots->Get())
			* (ipow(2, (int)m_microsteps->Get()));
	m_position = 0;
}

void Sequence::Loop() {
	unsigned long now = millis();
	if (m_shotsRemaining > 0) {

		if (now >= m_nextMove) {
			m_nextMove += (m_interval->Get() * 1000);
			stepper->step((int)m_stepsPerMovement);
			m_position += m_stepsPerMovement;
		}

		if (now >= m_nextTrigger) {
			m_nextTrigger += (m_interval->Get() * 1000);
			Trigger();
			m_shotsRemaining--;
		}
	} else if (m_position > 0) {
		// move to start
		if (now >= m_nextMove && !stepper->isMoving()) {
			stepper->step(-1 * m_stepsPerMovement);
			m_position -= m_stepsPerMovement;
		}
	}
}

void Sequence::OnDeviceInitializedState(PTP *ptp) {
	state = stConnected;
}

void Sequence::OnDeviceDisconnectedState(PTP *ptp) {
	state = stDisconnected;
}
