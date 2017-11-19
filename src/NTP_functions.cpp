
#include "NTP_functions.h"
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <TimeLib.h>
#include <NexRtc.h>

static const char ntpServerName[] = "de.pool.ntp.org"; // NTP Server
static const int timeZone = 2;     // Central European Time
static const unsigned int localPort = 8888;  // local port to listen for UDP packets
static const unsigned int syncTimeIntervall = 300;
static time_t prevDisplay = 0; // when the digital clock was updated

static const int NTP_PACKET_SIZE = 48; // NTP time is in the first 48 bytes of message
static byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets
static WiFiUDP ntpUDP;
static NexRtc rtc;

// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress &address)
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12] = 49;
  packetBuffer[13] = 0x4E;
  packetBuffer[14] = 49;
  packetBuffer[15] = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  ntpUDP.beginPacket(address, 123); //NTP requests are to port 123
  ntpUDP.write(packetBuffer, NTP_PACKET_SIZE);
  ntpUDP.endPacket();
}

time_t getNtpTime()
{
  IPAddress ntpServerIP; // NTP server's ip address

  while (ntpUDP.parsePacket() > 0) ; // discard any previously received packets
  Serial.println("Transmit NTP Request");
  // get a random server from the pool
  WiFi.hostByName(ntpServerName, ntpServerIP);
  Serial.print(ntpServerName);
  Serial.print(": ");
  Serial.println(ntpServerIP);
  sendNTPpacket(ntpServerIP);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500) {
    int size = ntpUDP.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      Serial.println("Receive NTP Response");
      ntpUDP.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
    }
  }
  Serial.println("No NTP Response :-(");
  return 0; // return 0 if unable to get the time
}

void stopNTP() {
    ntpUDP.stop();
    setSyncProvider(0);
}

void startNTP() {
    Serial.println("Starting UDP for NTP");
    ntpUDP.begin(localPort);
    Serial.print("Local port: ");
    Serial.println(ntpUDP.localPort());

    Serial.println("waiting for sync");
    setSyncProvider(getNtpTime);
    setSyncInterval(syncTimeIntervall);
}

void updateRTCTime()
{
    if (prevDisplay == 0 || (millis() - prevDisplay) > syncTimeIntervall * 1000) {
        now(); // update the time
        uint32_t time[7] = {
            static_cast<uint32_t>(year()),
            static_cast<uint32_t>(month()),
            static_cast<uint32_t>(day()),
            static_cast<uint32_t>(hour()),
            static_cast<uint32_t>(minute()),
            static_cast<uint32_t>(second())
        };

        if (rtc.write_rtc_time(time)) {
            Serial.println("time sucessfull updated in RTC");
            prevDisplay = millis();
        }
    }
}
