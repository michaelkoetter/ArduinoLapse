#include "Config.h"
#include "LiquidTWI2.h"
#include "utils.h"

ConfigValue::ConfigValue(long defaultValue,
		long min, long max, PrintFunction print, OnChange onChange)
		: m_value(defaultValue), m_min(min), m_max(max), m_print(print), m_change(onChange)
{
}

int ConfigValue::PrintValue(Print& print) {
	if (m_print != NULL) {
		return (*m_print)(m_value, print);
	}

	return print.print(m_value);
}

void ConfigValue::Modify(long amount) {
	long temp = m_value + amount;
	if (temp < m_min) temp = m_min;
	if (m_max != 0 && temp > m_max) temp = m_max;
	m_value = temp;
	if (m_change != NULL) {
		m_change(m_value);
	}
}


/**
 * Render user friendly time representation
 */
int PrintTime(long& value, Print& print) {
	size_t size = 0;
	long remain = value;

	// hours
	long current = remain / 3600;
	remain -= current * 3600;
	size += print.print(current);
	size += print.print(F("h "));

	// minutes
	current = remain / 60; //
	remain -= current * 60;
	size += print.print(current);
	size += print.print(F("' "));

	// seconds remain
	size += print.print(remain);
	size += print.print(F("\""));

	return size;
}

int PrintBacklightColor(long& value, Print& print) {
	switch(value) {
	case OFF:
		return print.print(F("Off"));
	case RED:
		return print.print(F("Red"));
	case YELLOW:
		return print.print(F("Yellow"));
	case GREEN:
		return print.print(F("Green"));
	case TEAL:
		return print.print(F("Teal"));
	case BLUE:
		return print.print(F("Blue"));
	case VIOLET:
		return print.print(F("Violet"));
	case WHITE:
		return print.print(F("White"));
	default:
		return print.print(F("Unknown"));
	}
}

int PrintTriggerMode(long& value, Print& print) {
	switch(value) {
	case TRIGGER_CABLE:
		return print.print(F("Cbl. Release"));
	case TRIGGER_PTP:
		return print.print(F("USB/PTP"));
	default:
		return print.print(F("Unknown"));
	}
}

int PrintMicrosteps(long& value, Print& print){
	return print.print(ipow(2, (int)value));
}

int PrintCurrent(long& value, Print& print)  {
	size_t size = print.print(value);
	size += print.print(F(" mA"));
}

