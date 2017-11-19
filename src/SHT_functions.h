
#ifndef SHT_FUNCTIONS_H
#define SHT_FUNCTIONS_H

#include <inttypes.h>

extern "C" {
/// SHT ///
uint8_t SHTget();
float getSHTTemp();
float getSHTHumidity();

}

#endif /* #ifndef SHT_FUNCTIONS_H */
