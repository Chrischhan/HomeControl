
#ifndef BMP180_FUNCTIONS_H
#define BMP180_FUNCTIONS_H

#include <inttypes.h>

extern "C" {
/// SHT ///
void initBMP180();
void getBMP180();
double getBMP180Temp();
double getBMP180Pressure();
double getBMP180PressureSealevel();
double getBMP180PressureAltitude();


}

#endif /* #ifndef BMP180_FUNCTIONS_H */
