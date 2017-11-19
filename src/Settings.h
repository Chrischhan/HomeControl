#ifndef SETTINGS_H
#define SETTINGS_H

#include <Arduino.h>

class Settings
{
public:
    Settings();
    String getSSID() const;
    void setSSID(const String& ssid);

    String getWifiPassword() const;
    void setWifiPassword(const String& pass);

    bool saveConfiguration() const;
    bool loadConfiguration();

private:

    String m_ssid; //  your network SSID (name)
    String m_wifipass; // your network password
};

#endif /* #ifndef SETTINGS_H */
