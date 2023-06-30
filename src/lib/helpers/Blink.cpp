#include "Blink.h"
void blink_all(int time_between)
{
    digitalWrite(Leds::BLUE, HIGH);
    digitalWrite(Leds::RED, HIGH);
    digitalWrite(Leds::GREEN, HIGH);
    digitalWrite(Leds::YELLOW, HIGH);
    delay(time_between);
    digitalWrite(Leds::BLUE, LOW);
    digitalWrite(Leds::RED, LOW);
    digitalWrite(Leds::GREEN, LOW);
    digitalWrite(Leds::YELLOW, LOW);
}