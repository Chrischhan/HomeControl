#include <Arduino.h>

#include "Settings.h"

//#include <Adafruit_NeoPixel.h>
//#include <Nextion.h>

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>

//#include <WiFiUdp.h>
#include <ArduinoOTA.h>

//#include <Wire.h>
//#include <RCSwitch.h>

//#include "libs.h"

Settings settings;
ESP8266WebServer server(80);

void handle404() {
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += server.uri();
    message += "\nMethod: ";
    message += (server.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += server.args();
    message += "\n";
    for (uint8_t i = 0; i < server.args(); i++)
    {
        message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }
    server.send(404, "text/plain", message);
}

void handleRoot()
{
    server.send(200, "text/plain", "hello from esp8266!");
}

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
//Adafruit_NeoPixel pixels = Adafruit_NeoPixel(8, D3, NEO_GRB + NEO_KHZ800);

// Manage network disconnection
void onSTADisconnected(WiFiEventStationModeDisconnected event_info)
{
    Serial.printf("Disconnected from SSID: %s\n", event_info.ssid.c_str());
    Serial.printf("Reason: %d\n", event_info.reason);
    //stopNTP();
}

//RCSwitch mySwitch = RCSwitch();

// converts the dBm to a range between 0 and 100%
int8_t getWifiQuality()
{
    int32_t dbm = WiFi.RSSI();
    if (dbm <= -100)
    {
        return 0;
    }
    else if (dbm >= -50)
    {
        return 100;
    }
    else
    {
        return 2 * (dbm + 100);
    }
}

void setup()
{
    Serial.begin(115200);
    /*if (not NextionInit())
    {
        Serial.println("");
        Serial.println("");
        Serial.println("Error in Nextion Init");
    }*/

    //Wire.begin();
    //pixels.begin(); // This initializes the NeoPixel library.

    /*for (int l = 0; l < 8; l++)
    {
        pixels.setPixelColor(l, pixels.Color(0, 0, 0));
    }
    pixels.show();

    delay(2500);
*/
    static WiFiEventHandler disconnect;

    Serial.println("");
    Serial.println("");
    // We start by connecting to a WiFi network
    Serial.print("try connecting to ");
    Serial.println(settings.getSSID());
    WiFi.mode(WIFI_STA);
    WiFi.begin(settings.getSSID().c_str(), settings.getWifiPassword().c_str());

    disconnect = WiFi.onStationModeDisconnected(onSTADisconnected);

    unsigned int retries = 10;

    while (WiFi.status() != WL_CONNECTED && retries > 0)
    {
        delay(500);
        Serial.print(".");
        retries--;
    }
    Serial.println("");

    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("Connection Failed! Rebooting...");
        // TODO: start in station Mode
        delay(5000);
        ESP.restart();
    }

    Serial.println("");

    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    //startNTP();
    //updateRTCTime();
    //initBMP180();

    // Port defaults to 8266
    // ArduinoOTA.setPort(8266);

    // Hostname defaults to esp8266-[ChipID]
    // ArduinoOTA.setHostname("myesp8266");

    // No authentication by default
    // ArduinoOTA.setPassword("admin");

    // Password can be set with it's md5 value as well
    // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
    // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

    ArduinoOTA.onStart([]() {
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH)
            type = "sketch";
        else // U_SPIFFS
            type = "filesystem";

        // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
        Serial.println("Start updating " + type);
    });

    ArduinoOTA.onEnd([]() {
        Serial.println("\nEnd");
    });

    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });

    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR)
            Serial.println("Auth Failed");
        else if (error == OTA_BEGIN_ERROR)
            Serial.println("Begin Failed");
        else if (error == OTA_CONNECT_ERROR)
            Serial.println("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR)
            Serial.println("Receive Failed");
        else if (error == OTA_END_ERROR)
            Serial.println("End Failed");
    });
    ArduinoOTA.begin();

    if (MDNS.begin("HomeControl")) 
    {
        Serial.println("MDNS responder started");
    }

    // Add service to MDNS-SD
    MDNS.addService("http", "tcp", 80);

    server.on("/", handleRoot);
    server.onNotFound(handle404);

    server.begin();


    //mySwitch.enableReceive(D8); // Receiver on interrupt 0 => that is pin #2
    //mySwitch.enableTransmit(D7);
}

//static const char *bin2tristate(const char *bin);
//static char *dec2binWzerofill(unsigned long Dec, unsigned int bitLength);

/*void output(unsigned long decimal, unsigned int length, unsigned int delay, unsigned int *raw, unsigned int protocol)
{

    if (decimal == 0)
    {
        Serial.print("Unknown encoding.");
    }
    else
    {
        const char *b = dec2binWzerofill(decimal, length);
        Serial.print("Decimal: ");
        Serial.print(decimal);
        Serial.print(" (");
        Serial.print(length);
        Serial.print("Bit) Binary: ");
        Serial.print(b);
        Serial.print(" Tri-State: ");
        Serial.print(bin2tristate(b));
        Serial.print(" PulseLength: ");
        Serial.print(delay);
        Serial.print(" microseconds");
        Serial.print(" Protocol: ");
        Serial.println(protocol);
    }

    //Serial.print("Raw data: ");
  //for (unsigned int i=0; i<= length*2; i++) {
    //Serial.print(raw[i]);
    //Serial.print(",");
}
    //Serial.println();
    //Serial.println();
}

static const char *bin2tristate(const char *bin)
{
    static char returnValue[50];
    int pos = 0;
    int pos2 = 0;
    while (bin[pos] != '\0' && bin[pos + 1] != '\0')
    {
        if (bin[pos] == '0' && bin[pos + 1] == '0')
        {
            returnValue[pos2] = '0';
        }
        else if (bin[pos] == '1' && bin[pos + 1] == '1')
        {
            returnValue[pos2] = '1';
        }
        else if (bin[pos] == '0' && bin[pos + 1] == '1')
        {
            returnValue[pos2] = 'F';
        }
        else
        {
            return "not applicable";
        }
        pos = pos + 2;
        pos2++;
    }
    returnValue[pos2] = '\0';
    return returnValue;
}

static char *dec2binWzerofill(unsigned long Dec, unsigned int bitLength)
{
    static char bin[64];
    unsigned int i = 0;

    while (Dec > 0)
    {
        bin[32 + i++] = ((Dec & 1) > 0) ? '1' : '0';
        Dec = Dec >> 1;
    }

    for (unsigned int j = 0; j < bitLength; j++)
    {
        if (j >= bitLength - i)
        {
            bin[j] = bin[31 + i - (j - (bitLength - i))];
        }
        else
        {
            bin[j] = '0';
        }
    }
    bin[bitLength] = '\0';

    return bin;
}*/

void loop()
{

    //updateRTCTime();

    ArduinoOTA.handle();
    server.handleClient();
    /*unsigned long start = millis();
    while (millis() - 20000 < start)
    {
        int value = analogRead(A0);    //Ließt den Wert des Sensors vom Analogen PIN2
        Serial.println(value);
        delay(100);
    }*/

    /*for (int l = 0; l < 8; l++)
    {
        pixels.setPixelColor(l, pixels.Color(0, 0, 0));
    }
    pixels.show();*/

    //pixels.setPixelColor(0, pixels.Color( 255,0, 0));
    //pixels.setPixelColor(1, pixels.Color(0, 255,0));
    //pixels.setPixelColor(2, pixels.Color(0, 0, 255));
    //pixels.setPixelColor(3, pixels.Color(0, 0, 0));
    //pixels.show();
    //delay(2000);
    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    /*for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 2; j++) {
      for (int k = 0; k < 2; k++) {
        for (int l = 0; l < 8; l++) {
          pixels.setPixelColor(l, pixels.Color(i * 255, j * 255, k * 255)); // Moderately bright green color.
        }
        //pixels.setPixelColor(0, pixels.Color(i * 255, j * 255, k * 255)); // Moderately bright green color.

        pixels.show(); // This sends the updated pixel color to the hardware.

        delay(200); // Delay for a period of time (in milliseconds).
      }
    }
}*/

    /*Serial.println("Scanning...");
 if (SHTget() == 0) {
     Serial.print("Temperature in Celsius : ");
         Serial.println(getSHTTemp());
         Serial.print("Relative Humidity : ");
         Serial.println(getSHTHumidity());
         Serial.println();
 }

 getBMP180();
 Serial.print("BMP180 Temp : ");
 Serial.println(getBMP180Temp());
 Serial.print("BMP180 Pressure : ");
 Serial.println(getBMP180Pressure());
 Serial.print("BMP180 Sealevel : ");
 Serial.println(getBMP180PressureSealevel());
 Serial.print("BMP180 Altitude : ");
 Serial.println(getBMP180PressureAltitude());
*/

    /*if (mySwitch.available())
    {
        output(mySwitch.getReceivedValue(), mySwitch.getReceivedBitlength(), mySwitch.getReceivedDelay(), mySwitch.getReceivedRawdata(), mySwitch.getReceivedProtocol());
        mySwitch.resetAvailable();
    }*/

    //        FFF0 0FFFFFF 0
    //        BAND DEVICE  ON/OFF
    // 1 On : FFF00FFFFFFF
    // 1 Off: FFF00FFFFFF0
    // 2 On : FFF0F0FFFFFF
    // 2 Off: FFF0F0FFFFF0
    // 3 On : FFF0FF0FFFFF
    // 3 Off: FFF0FF0FFFF0
    // 4 On : FFF0FFF0FFFF
    // 4 Off: FFF0FFF0FFF0

    //mySwitch.sendTriState("FFF0FF0FFFF0");
    //delay(1000);
    //mySwitch.sendTriState("FFF0FF0FFFFF");
    //delay(1000);

    delay(1000);
}
