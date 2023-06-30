#include "InitialSetup.h"

StaticJsonDocument<128> code_response;
StaticJsonDocument<128> config_read_setup;

void RunInitialSetup()
{
    if (LittleFS.exists("/creds"))
    {
        File cred_file = LittleFS.open("/creds", "r");
        deserializeJson(config_read_setup, cred_file);
        cred_file.close();
        HTTPClient http;
        WiFiClientSecure client;
        client.setInsecure();
        const char* id = config_read_setup["id"];
        const char* secret_key = config_read_setup["secret_key"];
        String url = "https://mawoka-myblock.gh.srv.us/api/v1/box-controller/embedded/ping?id=";
        url += id;
        url += "&secret_key=";
        url += secret_key;
        url += "&version=";
        url += VERSION;
        http.begin(client, url);
        int respCode = http.GET();
        Serial.println(respCode);
        if (respCode == 404) {
            LittleFS.remove("/creds");
            ESP.restart();
        }
        // doc["id"] = config_read["id"];
        // doc["secret_key"] = config_read["secret_key"];
        return;
    }
    bool success = false;
    while (!success)
    {
        HTTPClient http;
        WiFiClientSecure client;
        client.setInsecure();
        String code = get_code_as_str(10);
        Serial.println(code);
        http.begin(client, "https://mawoka-myblock.gh.srv.us/api/v1/box-controller/embedded/register");
        http.addHeader("Content-Type", "application/json");
        String payload = "{\"code\":\"";
        payload += code;
        payload += "\"}";
        int respCode = http.POST(payload);
        if (respCode != 200)
        {
            Serial.println(respCode);
            continue;
        }
        deserializeJson(code_response, http.getStream());
        File cred_file = LittleFS.open("/creds", "w");
        serializeJson(code_response, cred_file);
        cred_file.close();
        success = true;
        Serial.println("Connected!");
    }
}