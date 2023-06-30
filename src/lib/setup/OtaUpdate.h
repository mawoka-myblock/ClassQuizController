#ifndef OTAUPDATE_H
#define OTAUPDATE_H

#include <Arduino.h>
#include "Config.h"
#include <WiFiClientSecure.h>
#include <LittleFS.h>
#include "GetCode.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Update.h>
#include "Blink.h"


void RunOTAUpdateIfAvailable();

#endif