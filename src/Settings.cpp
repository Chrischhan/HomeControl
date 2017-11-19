#include "Settings.h"
#include <FS.h>

Settings::Settings()
: m_ssid("")
, m_wifipass("") {}

String Settings::getSSID() const {
    return m_ssid;
}

void Settings::setSSID(const String& ssid) {
    m_ssid = ssid;
}

String Settings::getWifiPassword() const {
    return m_wifipass;
}

void Settings::setWifiPassword(const String& pass) {
    m_wifipass = pass;
}

bool Settings::saveConfiguration() const {
    File file = SPIFFS.open("/HomeControl.txt", "w+");

    if (not file) {
        Serial.println("Failed to open config file");
        return false;
    }

    file.print("WIFI_SSID=");
    file.println(m_ssid);
    file.print("WIFI_PASS=");
    file.println(m_wifipass);

    file.close();
    Serial.println("Saved values");
    return true;
}

bool Settings::loadConfiguration() {
    File file = SPIFFS.open("/HomeControl.txt", "r");

    if (not file) {
        Serial.println("Failed to open config file");
        return false;
    }

    while(file.available()) {
        //Lets read line by line from the file
        String key = file.readStringUntil('=');
        String value = file.readStringUntil('\r');
        file.read();

        Serial.println(key + " = [" + value + "]");
        Serial.println(key.length());

        if (key == "WIFI_SSID") {
            setSSID(value);
        }

        if (key == "WIFI_PASS") {
            setWifiPassword(value);
        }
    }

    file.close();
    Serial.println("Loaded config");
    return true;
}
