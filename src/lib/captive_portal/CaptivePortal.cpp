#include "CaptivePortal.h"

const byte DNS_PORT = 53;
IPAddress apIP(8, 8, 4, 4); // The default android DNS
DNSServer dnsServer;
AsyncWebServer server(80);
StaticJsonDocument<96> config_data;

String responseHTML = "<!DOCTYPE html> <html lang='en'> <head> <meta charset='UTF-8' /> <meta http-equiv='X-UA-Compatible' content='IE=edge' /> <meta name='viewport' content='width=device-width, initial-scale=1.0' /> <title>Set ClassQuizController up</title> </head> <body> <h1 style='text-align: center'>ClassQuizController</h1> <form action='GET'> <input type='text' placeholder='WLAN SSID' required name='ssid' /> <br /> <input type='password' placeholder='WLAN password' required name='password' /> <button type='submit'>Submit</button> </form> </body> </html> <style> form { display: flex; justify-content: center; flex-direction: column; } </style>";

void connect_to_wifi(const char *ssid, const char *password)
{
    Serial.println("Trying to connect to WLAN");
    WiFi.begin(ssid, password);
    for (int i = 0; i < 5; i++)
    {
        if (WiFi.status() == WL_CONNECTED)
        {
            break;
        }
        else
        {
            delay(500);
        }
    }
    if (WiFi.status() != WL_CONNECTED)
    {
        return;
    }
    config_data["ssid"] = ssid;
    config_data["password"] = password;
    File conf_file = LittleFS.open("/wifi_conf", "w");
    serializeJson(config_data, conf_file);
    conf_file.close();
    ESP.restart();
    return;
}

class CaptiveRequestHandler : public AsyncWebHandler
{
public:
    CaptiveRequestHandler() {}
    virtual ~CaptiveRequestHandler() {}

    bool canHandle(AsyncWebServerRequest *request)
    {
        // request->addInterestingHeader("ANY");
        return true;
    }

    void handleRequest(AsyncWebServerRequest *request)
    {
        if (request->hasParam("ssid") && request->hasParam("password"))
        {
            const char *ssid = request->getParam("ssid")->value().c_str();
            const char *password = request->getParam("password")->value().c_str();
            connect_to_wifi(ssid, password);
        }
        else
        {
            Serial.println("GET!!!!!");
            AsyncResponseStream *response = request->beginResponseStream("text/html");
            response->print(responseHTML);
            request->send(response);
        }
    }
};
String getValue(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = {0, -1};
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++)
    {
        if (data.charAt(i) == separator || i == maxIndex)
        {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i + 1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void CaptivePortalSetup()
{
    if (LittleFS.exists("/wifi_conf"))
    {
        File conf_file = LittleFS.open("/wifi_conf", "r");
        DeserializationError error = deserializeJson(config_data, conf_file);
        conf_file.close();
        if (!error)
        {
            WiFi.mode(WIFI_STA);
            const char *ssid = config_data["ssid"];
            const char *password = config_data["password"];
            WiFi.begin(ssid, password);
            for (int i = 0; i < 5; i++)
            {
                if (WiFi.status() == WL_CONNECTED)
                {
                    break;
                }
                else
                {
                    delay(500);
                }
            }
            if (WiFi.status() != WL_CONNECTED)
            {
                Serial.println("WiFi connection failed... Resetting");
                LittleFS.remove("/wifi_conf");
                ESP.restart();
                return;
            }
            Serial.println("Successfully connected to WiFi!");
            return;
        }
    }
    WiFi.mode(WIFI_AP);
    WiFi.softAP("ClassQuizController");
    WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));

    // if DNSServer is started with "*" for domain name, it will reply with
    // provided IP to all DNS request
    dnsServer.start(DNS_PORT, "*", apIP);
    server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);
    bool ready_for_wifi = true;
    server.begin();
    Serial.println("This loop");
    while (true)
    {
        if (ready_for_wifi)
        {
            Serial.println("Ready for WiFi");
        }
        if (Serial.available() > 0)
        {
            // read the incoming string:
            String incomingString = Serial.readStringUntil('\n');

            // prints the received data
            Serial.print("I received: ");
            Serial.println(incomingString);
            String command = getValue(incomingString, ' ', 0);
            ready_for_wifi = false;
            if (command == "connect")
            {
                ready_for_wifi = false;
                const char *wifi_ssid = getValue(incomingString, ' ', 1).c_str();
                const char *wifi_password = getValue(incomingString, ' ', 2).c_str();
                connect_to_wifi(wifi_ssid, wifi_password);
                ready_for_wifi = true;
            }
        }
        dnsServer.processNextRequest();
    }
}
