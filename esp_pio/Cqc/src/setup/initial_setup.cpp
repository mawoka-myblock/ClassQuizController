#include "initial_setup.h"

StaticJsonDocument<128> success_register_response;

void set_controller_up()
{

    const char *code = get_code_as_str(10).c_str();
    Serial.println(code);
    if (WiFi.status() == WL_CONNECTED)
    {
        WiFiClient wifi;
        std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);
        client->setInsecure();
        HTTPClient https;
        Serial.println(WiFi.localIP());
        // if (https.begin(*client, "http://192.168.2.243:8080/api/v1/box-controller/embedded/register"))
        if (https.begin(*client, "https://classquiz.de/api/v1/box-controller/embedded/register"))

        {
            Serial.println("Posting...");
            Serial.println(ESP.getFreeHeap());
            https.addHeader("Content-Type", "application/json");
            String json = "{\"code\":\"";
            json += code;
            json += "\"}";
            int resp_code = https.POST(json);
            if (resp_code > 0)
            {
                // HTTP header has been send and Server response header has been handled
                Serial.printf("[HTTP] POST... code: %d\n", resp_code);

                // file found at server
                if (resp_code == HTTP_CODE_OK)
                {
                    const String &payload = https.getString();
                    deserializeJson(success_register_response, payload);
                    File conf_file = LittleFS.open("/conf", "w");
                    const char *id = success_register_response["id"];
                    const char *secret_key = success_register_response["secret_key"];
                    String conf_str = id;
                    conf_str += "\n";
                    conf_str += secret_key;
                    conf_file.print(conf_str);
                    conf_file.close();
                }
            }
            else
            {
                Serial.printf("[HTTP] POST... failed, error: %s\n", https.errorToString(resp_code).c_str());
            }
            https.end();
        }
    }
    else
    {
        Serial.println("WiFi not connected!");
    }
}