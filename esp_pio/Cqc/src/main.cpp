#include <Arduino.h>
#include "config.h"
#include "setup/get_code.h"
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <AutoConnect.h>
#include <LittleFS.h>
#include "setup/initial_setup.h"

ESP8266WebServer Server;
AutoConnect portal;
AutoConnectConfig config;

void setup()
{
    Serial.begin(9600);
    pinMode(Leds::BLUE, OUTPUT);            // Blue LED
    pinMode(Leds::GREEN, OUTPUT);           // Green LED
    pinMode(Leds::YELLOW, OUTPUT);          // Yellow LED
    pinMode(Leds::RED, OUTPUT);             // Red LED
    pinMode(Buttons::BLUE, INPUT_PULLUP);   // Blue Button
    pinMode(Buttons::GREEN, INPUT_PULLUP);  // Green Button
    pinMode(Buttons::YELLOW, INPUT_PULLUP); // Yellow Button
    pinMode(Buttons::RED, INPUT_PULLUP);    // Red Button

    config.apid = "ClassQuizController";
    config.psk = "ClassQuiz";
    portal.config(config);

    if (!LittleFS.begin())
    {
        LittleFS.format();
        ESP.restart();
    }

    portal.restoreCredential("/cred", LittleFS);
    if (portal.begin())
    {
        portal.saveCredential("/creds", LittleFS);
    }
    File conf_file = LittleFS.open("/conf", "r");
    if (!conf_file)
    {
        Serial.println("Setup!");
        set_controller_up();
    }
    else
    {
        while (conf_file.available())
        {
            Serial.write(conf_file.read());
        }
    }
}

String get_session_id()
{
    String session_id = "";
    while (session_id.length() == 0)
    {
        String code = get_code_as_str(6);
    }
    return session_id;
}

void loop()
{
    String sid = get_session_id();
}