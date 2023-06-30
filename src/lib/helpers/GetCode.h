#ifndef GET_CODE_H
#define GET_CODE_H

#include <Arduino.h>
#include <Config.h>

String get_code_as_str(int code_length);
void long_blink(int pin);
void short_blink(int pin);


#endif // GET_CODE_H