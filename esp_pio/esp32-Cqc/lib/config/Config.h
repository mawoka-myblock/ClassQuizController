#ifndef CONFIG_H
#define CONFIG_H

#define VERSION "0.0.0"

#include <Arduino.h>
#include <LittleFS.h>

class Leds
{
public:
    static int BLUE;
    static int GREEN;
    static int YELLOW;
    static int RED;
};

class Buttons
{
public:
    static int BLUE;
    static int GREEN;
    static int YELLOW;
    static int RED;
};


#endif // CONFIG_H