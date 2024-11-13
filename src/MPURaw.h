#ifndef MPURAW_H
#define MPURAW_H

#include <Arduino.h>

#define iic_ad 0x68 // MPU6050 address

// Function prototypes
void mpuraw_init(void);
int16_t mpuraw(uint8_t index);

#endif
