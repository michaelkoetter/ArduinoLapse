#ifndef CONFIG_H_
#define CONFIG_H_

#include "Print.h"

#define TRIGGER_CABLE 0
#define TRIGGER_PTP 1

typedef int (*PrintFunction) (long& value, Print& print);

// pretty print a time in seconds, minutes, hours
int PrintTime(long& value, Print& print);

// pretty print the backlight color
int PrintBacklightColor(long& value, Print& print);

// pretty print the trigger mode
int PrintTriggerMode(long& value, Print& print);

// translate microsteps 1,2,3,.. = 1,2,4,...
int PrintMicrosteps(long& value, Print& print);

// print current in milliamps  xxx mA
int PrintCurrent(long& value, Print& print);

class ConfigValue {
public:
	ConfigValue(long defaultValue, long min = 0, long max = 0, PrintFunction print = NULL);
	void Modify(long amount);

	int PrintValue(Print& print);

	long Get() const { return m_value; }
	void Set(long value) { m_value = value; Modify(0); }

	int GetMin() const { return m_min; }
	void SetMin(long min) { m_min = min; Modify(0); }

	int GetMax() const { return m_max; }
	void SetMax(long max) { m_max = max; Modify(0); }

protected:
	long		m_value;
	long		m_min, m_max;
	PrintFunction	m_print;
};

#endif /* CONFIG_H_ */
