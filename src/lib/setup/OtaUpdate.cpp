#include "OtaUpdate.h"

StaticJsonDocument<128> config_read_setup_for_update;

DynamicJsonDocument resp_data(6144);
bool led_on = false;

void progressCallback(size_t currSize, size_t totalSize)
{
    Serial.printf("CALLBACK:  Update process at %d of %d bytes...\n", currSize, totalSize);
    if (led_on)
    {
        digitalWrite(Leds::RED, LOW);
    }
    else
    {
        digitalWrite(Leds::RED, HIGH);
    }
    led_on = !led_on;
}

void RunOTAUpdateIfAvailable()
{
    Serial.println("Checking for update");
    Serial.println(true);
    File cred_file = LittleFS.open("/creds", "r");
    deserializeJson(config_read_setup_for_update, cred_file);
    cred_file.close();
    HTTPClient http;
    WiFiClientSecure client;
    client.setInsecure();
    const char *id = config_read_setup_for_update["id"];
    const char *secret_key = config_read_setup_for_update["secret_key"];
    String url = "https://mawoka-myblock.gh.srv.us/api/v1/box-controller/embedded/update?id=";
    url += id;
    url += "&secret_key=";
    url += secret_key;
    http.begin(client, url);
    int respCode = http.GET();
    Serial.println(respCode);
    if (respCode != 200)
    {
        return;
    }
    String wanted_os_version = http.getString();
    if (wanted_os_version == VERSION)
    {
        // No update needed, already on wanted version
        Serial.println("No update needed");
        return;
    }
    url = "https://api.github.com/repos/mawoka-myblock/ClassQuizController/releases/tags/v";
    url += wanted_os_version;
    http.begin(client, url);
    respCode = http.GET();
    if (respCode != 200)
    {
        return;
    }
    DeserializationError error = deserializeJson(resp_data, http.getStream());

    if (error)
    {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        return;
    }
    long release_id = resp_data["id"];
    url = "https://api.github.com/repos/mawoka-myblock/ClassQuizController/releases/";
    url += release_id;
    url += "/assets";
    http.begin(client, url);
    respCode = http.GET();
    Serial.println("L74");
    Serial.println(respCode);
    if (respCode != 200)
    {
        ESP.restart();
    }
    deserializeJson(resp_data, http.getStream());
    String hash = "";
    for (JsonObject item : resp_data.as<JsonArray>())
    {
        const char *name = item["name"];
        http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
        http.begin(client, item["browser_download_url"]);
        int respCode = http.GET();
        Serial.println("L88");
        Serial.println(respCode);
        Serial.println(name);
        const char *fw_filename = "firmware.bin";
        const char *hash_filename = "HASH";
        if (respCode != 200)
        {
            return;
        }
        if (strcmp(fw_filename, name) == 0)
        {
            File update_file = LittleFS.open("/update.bin", "w");
            http.writeToStream(&update_file);
            update_file.close();
        }
        else if (strcmp(hash_filename, name) == 0)
        {
            hash = http.getString();
        }
        else
        {
            continue;
        }
    }
    if (/*hash == "" || */ !LittleFS.exists("/update.bin"))
    {
        Serial.println("Hash and/or update file missing!");
    }
    File firmware = LittleFS.open("/update.bin", "r");
    Serial.println(firmware.size());
    Update.onProgress(progressCallback);
    Update.begin(firmware.size(), U_FLASH);
    Update.writeStream(firmware);
    if (Update.end())
    {
        Serial.println(F("Update finished!"));
    }
    else
    {
        Serial.println(F("Update error!"));
        Serial.println(Update.getError());
    }
    firmware.close();
    digitalWrite(Leds::RED, LOW);
    blink_all(200);
    delay(200);
    blink_all(200);
    LittleFS.remove("/update.bin");
    ESP.restart();
}