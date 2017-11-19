
#include "SHT_functions.h"
#include <Arduino.h>
#include <Wire.h>

static const uint8_t address = 0x45;
static float cTemp=0;
static float humidity=0;

float getSHTTemp() { return cTemp; }
float getSHTHumidity() { return humidity; }

uint8_t SHTget()
{
	unsigned int data[6];
	// Start I2C Transmission
	Wire.beginTransmission(address);
	// Send measurement command
	Wire.write(0x2C);
	Wire.write(0x06);
	// Stop I2C transmission
	if (Wire.endTransmission()!=0)
		return 1;

    unsigned long start = millis();
    while (millis() - 500 < start);

	// Request 6 bytes of data
	Wire.requestFrom(address, static_cast<size_t>(6));

	// Read 6 bytes of data
	// cTemp msb, cTemp lsb, cTemp crc, humidity msb, humidity lsb, humidity crc
	for (int i=0;i<6;i++) {
		data[i]=Wire.read();
	};

    start = millis();
    while (millis() - 50 < start);

	if (Wire.available()!=0)
		return 2;

	// Convert the data
	cTemp = ((((data[0] * 256.0) + data[1]) * 175) / 65535.0) - 45;
	humidity = ((((data[3] * 256.0) + data[4]) * 100) / 65535.0);

	return 0;
}
