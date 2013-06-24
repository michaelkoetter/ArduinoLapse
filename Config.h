#ifndef CONFIG_H_
#define CONFIG_H_

#include "Print.h"

#define TRIGGER_CABLE 0
#define TRIGGER_PTP 1

typedef int (*PrintFunction) (int& value, Print& print);

int PrintTime(int& value, Print& print);
int PrintBacklightColor(int& value, Print& print);
int PrintTriggerMode(int& value, Print& print);
int PrintMicrosteps(int& value, Print& print);
int PrintCurrent(int& value, Print& print);

class ConfigValue {
public:
	ConfigValue(int defaultValue, int min = 0, int max = 0, PrintFunction print = NULL)
		: m_value(defaultValue), m_min(min), m_max(max), m_print(print) {}

	void Modify(int amount);

	int PrintValue(Print& print);

	int Get() const { return m_value; }
	void Set(int value) { m_value = value; Modify(0); }

	int GetMin() const { return m_min; }
	void SetMin(int min) { m_min = min; Modify(0); }

	int GetMax() const { return m_max; }
	void SetMax(int max) { m_max = max; Modify(0); }

protected:
	int		m_value;
	int		m_min, m_max;
	PrintFunction	m_print;
};

#endif /* CONFIG_H_ */
