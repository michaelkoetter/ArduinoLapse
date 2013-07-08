#include "Sequence.h"

Sequence::Sequence(USB *usb, TMC26XStepper *stepper,
		ConfigValue *shots,
		ConfigValue *movement,
		ConfigValue *interval,
		ConfigValue *stabilize)
	: ptp(usb, this),
	  stepper(stepper),
	  state(stInitial),
	  m_shots(shots),
	  m_movement(movement),
	  m_interval(interval),
	  m_stabilize(stabilize),
	  m_nextMove(0),
	  m_nextTrigger(0),
	  m_shotsRemaining(0)
{
}


void Sequence::Trigger() {
	if (state == stConnected) {
		ptp.CaptureImage();
	}
}

void Sequence::Start() {
	unsigned long now = millis();
	m_shotsRemaining = m_shots->Get();

	m_nextMove = now;
	m_nextTrigger = m_nextMove + (m_stabilize->Get() * 1000);
}

void Sequence::Loop() {
	unsigned long now = millis();
	if (m_shotsRemaining > 0) {
		if (now >= m_nextTrigger) {
			m_nextTrigger += (m_interval->Get() * 1000);
			Trigger();
		}
	}

}

void Sequence::OnDeviceInitializedState(PTP *ptp) {
	state = stConnected;
}

void Sequence::OnDeviceDisconnectedState(PTP *ptp) {
	state = stDisconnected;
}
