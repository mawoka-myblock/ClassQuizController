#include "GetCode.h"

#define SHORT_PRESS_TIME 500 // in milliseconds

bool blue_button_state = false;
bool green_button_state = false;
bool yellow_button_state = false;
bool red_button_state = false;
bool red_long_press = false;
bool green_long_press = false;
bool blue_long_press = false;
bool yellow_long_press = false;

unsigned long blue_button_press_time = 0;
unsigned long green_button_press_time = 0;
unsigned long yellow_button_press_time = 0;
unsigned long red_button_press_time = 0;

void long_blink(int pin)
{
    digitalWrite(pin, HIGH);
    delay(600);
    digitalWrite(pin, LOW);
}

void short_blink(int pin)
{
    digitalWrite(pin, HIGH);
    delay(300);
    digitalWrite(pin, LOW);
}

String get_code_as_str(int code_length)
{
    const int PRESSED_STATE = 0;
    String code = "";
    while (code.length() < code_length)
    {
        // blue
        if (digitalRead(Buttons::BLUE) == PRESSED_STATE)
        {
            if (blue_button_state == false)
            {
                blue_button_state = true;
                blue_button_press_time = millis();
            }
            if ((millis() - blue_button_press_time > 1000) && (blue_long_press == false))
            {
                blue_long_press = true;
                code += "B";
                long_blink(Leds::BLUE);
            }
        }
        else
        {
            if (blue_button_state == true)
            {
                if (blue_long_press == true)
                {
                    blue_long_press = false;
                }
                else
                {
                    code += "b";
                    short_blink(Leds::BLUE);
                }
                blue_button_state = false;
            }
        }

        // Check the green button
        if (digitalRead(Buttons::GREEN) == PRESSED_STATE)
        {
            if (green_button_state == false)
            {
                green_button_state = true;
                green_button_press_time = millis();
            }
            if ((millis() - green_button_press_time > 1000) && (green_long_press == false))
            {
                green_long_press = true;
                code += "G";
                long_blink(Leds::GREEN);
            }
        }
        else
        {
            if (green_button_state == true)
            {
                if (green_long_press == true)
                {
                    green_long_press = false;
                }
                else
                {
                    code += "g";
                    short_blink(Leds::GREEN);
                }
                green_button_state = false;
            }
        }

        // Check the yellow button
        if (digitalRead(Buttons::YELLOW) == PRESSED_STATE)
        {
            if (yellow_button_state == false)
            {
                yellow_button_state = true;
                yellow_button_press_time = millis();
            }
            if ((millis() - yellow_button_press_time > 1000) && (yellow_long_press == false))
            {
                yellow_long_press = true;
                code += "Y";
                long_blink(Leds::YELLOW);
            }
        }
        else
        {
            if (yellow_button_state == true)
            {
                if (yellow_long_press == true)
                {
                    yellow_long_press = false;
                }
                else
                {
                    code += "y";
                    short_blink(Leds::YELLOW);
                }
                yellow_button_state = false;
            }
        }

        if (digitalRead(Buttons::RED) == PRESSED_STATE)
        {
            if (red_button_state == false)
            {
                red_button_state = true;
                red_button_press_time = millis();
            }
            if ((millis() - red_button_press_time > 1000) && (red_long_press == false))
            {
                red_long_press = true;
                code += "R";
                long_blink(Leds::RED);
            }
        }
        else
        {
            if (red_button_state == true)
            {
                if (red_long_press == true)
                {
                    red_long_press = false;
                }
                else
                {
                    code += "r";
                    short_blink(Leds::RED);
                }
                red_button_state = false;
            }
        }

        delay(50);
    }

    return code;
}