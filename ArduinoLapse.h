#ifndef ArduinoLapse_H_
#define ArduinoLapse_H_
#include "Arduino.h"

#ifdef __cplusplus
extern "C" {
#endif
void loop();
void setup();

// forward declarations
void on_change_motor_speed(long& value);
void on_change_motor_microsteps(long& value);
void on_change_backlight(long& value);

#ifdef __cplusplus
} // extern "C"
#endif


#endif /* ArduinoLapse_H_ */
