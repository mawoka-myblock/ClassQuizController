#include "CaptivePortal.h"

const byte DNS_PORT = 53;
IPAddress apIP(8, 8, 4, 4); // The default android DNS
DNSServer dnsServer;
AsyncWebServer server(80);
StaticJsonDocument<96> config_data;

String responseHTML = "<!DOCTYPE html> <html lang='en'> <head> <meta charset='UTF-8' /> <meta http-equiv='X-UA-Compatible' content='IE=edge' /> <meta name='viewport' content='width=device-width, initial-scale=1.0' /> <title>Set ClassQuizController up</title> </head> <body> <h1 style='text-align: center'>ClassQuizController</h1> <form action='GET'> <input type='text' placeholder='WLAN SSID' required name='ssid' /> <br /> <input type='password' placeholder='WLAN password' required name='password' /> <button type='submit'>Submit</button> </form> </body> </html> <style> form { display: flex; justify-content: center; flex-direction: column; } </style>";

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
            Serial.println("Trying to connect to WLAN");
            const char *ssid = request->getParam("ssid")->value().c_str();
            const char *password = request->getParam("password")->value().c_str();
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
        else
        {
            Serial.println("GET!!!!!");
            AsyncResponseStream *response = request->beginResponseStream("text/html");
            response->print(responseHTML);
            request->send(response);
        }
    }
};

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
    WiFi.softAP("ESP32-DNSServer");
    WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));

    // if DNSServer is started with "*" for domain name, it will reply with
    // provided IP to all DNS request
    dnsServer.start(DNS_PORT, "*", apIP);
    server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);

    server.begin();

    while (true)
    {
        dnsServer.processNextRequest();
    }
}
