#ifndef CONFIG_H_
#define CONFIG_H_

#include "Print.h"

template<class T>
class ConfigValue {
public:
	ConfigValue(T defaultValue, T min = 0, T max = 0)
		: m_value(defaultValue), m_min(min), m_max(max) {}

	virtual void Modify(T amount) {
		T temp = m_value + amount;
		if (temp < m_min) temp = m_min;
		if (m_max != 0 && temp > m_max) temp = m_max;
		m_value = temp;
	}

	T Get() const { return m_value; }
	void Set(T value) { m_value = value; Modify(0); }

	T GetMin() const { return m_min; }
	void SetMin(T min) { m_min = min; Modify(0); }

	T GetMax() const { return m_max; }
	void SetMax(T max) { m_max = max; Modify(0); }

protected:
	T	m_value;
	T 	m_min, m_max;
};

class Config {
};

#endif /* CONFIG_H_ */
