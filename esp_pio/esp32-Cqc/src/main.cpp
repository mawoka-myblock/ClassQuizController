#include <Arduino.h>
#include <WiFiClientSecure.h>
#include "Config.h"
#include "CaptivePortal.h"
#include "InitialSetup.h"
#include <WebSocketsClient.h>

WiFiClientSecure client;
StaticJsonDocument<192> doc;
StaticJsonDocument<128> config_read;
StaticJsonDocument<48> join_response;
WebSocketsClient webSocket;

bool keepConnection = true;

void setup()
{
    Serial.begin(9600);
    pinMode(Leds::BLUE, OUTPUT);
    pinMode(Leds::GREEN, OUTPUT);
    pinMode(Leds::YELLOW, OUTPUT);
    pinMode(Leds::RED, OUTPUT);
    pinMode(Buttons::BLUE, INPUT_PULLUP);
    pinMode(Buttons::GREEN, INPUT_PULLUP);
    pinMode(Buttons::YELLOW, INPUT_PULLUP);
    pinMode(Buttons::RED, INPUT_PULLUP);
    LittleFS.begin(true);
    CaptivePortalSetup();
    client.setInsecure();
    RunInitialSetup();
}

void webSocketEvent(WStype_t type, uint8_t *payload, size_t length)
{

    switch (type)
    {
    case WStype_DISCONNECTED:
        Serial.printf("[WSc] Disconnected!\n");
        break;
    case WStype_CONNECTED:
    {
        Serial.printf("[WSc] Connected to url: %s\n", payload);

        // send message to server when Connected
        // webSocket.sendTXT("Connected");
    }
    break;
    case WStype_TEXT:
        Serial.printf("[WSc] get text: %s\n", payload);

        // send message to server
        // webSocket.sendTXT("message here");
        break;
    case WStype_BIN:
        Serial.printf("[WSc] get binary length: %u\n", length);

        // send data to server
        // webSocket.sendBIN(payload, length);
        break;
    case WStype_ERROR:
    case WStype_FRAGMENT_TEXT_START:
    case WStype_FRAGMENT_BIN_START:
    case WStype_FRAGMENT:
    case WStype_FRAGMENT_FIN:
        break;
    }
}

void loop()
{
    String code = get_code_as_str(6);
    File cred_file = LittleFS.open("/creds", "r");
    deserializeJson(config_read, cred_file);
    cred_file.close();
    doc["id"] = config_read["id"];
    doc["secret_key"] = config_read["secret_key"];
    doc["code"] = code;
    HTTPClient http;
    http.begin(client, "https://mawoka-myblock.gh.srv.us/api/v1/box-controller/embedded/join");
    http.addHeader("Content-Type", "application/json");
    String payload;
    serializeJson(doc, payload);
    int respCode = http.POST(payload);
    Serial.println(respCode);
    if (respCode != 200)
    {
        return;
    }
    deserializeJson(join_response, http.getStream());
    client.setInsecure();
    String ws_url = "/api/v1/box-controller/embedded/socket/";
    String connection_id = join_response["id"];
    ws_url += connection_id;
    Serial.println("DONE WITH REQUEST");
    // webSocket.beginSSL("mawoka-myblock.gh.srv.us", 443, ws_url);
    webSocket.begin("192.168.178.103", 8080, ws_url, "");
    webSocket.onEvent(webSocketEvent);
    int neutral_button = digitalRead(Buttons::BLUE);

    while (keepConnection)
    {
        webSocket.loop();
        if (digitalRead(Buttons::BLUE) != neutral_button)
        {
            String json_payload = "{\"type\":\"bp\",\"data\":\"";
            json_payload += "b";
            json_payload += "\"}";
            webSocket.sendTXT(json_payload);
            delay(30);
        }
        if (digitalRead(Buttons::GREEN) != neutral_button)
        {
            String json_payload = "{\"type\":\"bp\",\"data\":\"";
            json_payload += "g";
            json_payload += "\"}";
            webSocket.sendTXT(json_payload);
            delay(30);
        }
        if (digitalRead(Buttons::YELLOW) != neutral_button)
        {
            String json_payload = "{\"type\":\"bp\",\"data\":\"";
            json_payload += "y";
            json_payload += "\"}";
            webSocket.sendTXT(json_payload);
            delay(30);
        }
        if (digitalRead(Buttons::RED) != neutral_button)
        {
            String json_payload = "{\"type\":\"bp\",\"data\":\"";
            json_payload += "r";
            json_payload += "\"}";
            webSocket.sendTXT(json_payload);
            delay(30);
        }
    }
}