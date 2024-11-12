#include "MPURaw.h"

#define DWANPORTB *((volatile uint8_t*)0x25)
#define DWANDIRB *((volatile uint8_t*)0x24)
#define DWANPINB *((volatile uint8_t*)0x23)

void i2c_init() {
  DWANDIRB |= (1 << 4) | (1 << 3);
  DWANPORTB |= (1 << 4) | (1 << 3);  // Starts the I2C bus high
}

void i2c_start() {
  DWANPORTB |= (1 << 4);
  for (int i = 0; i < 100; i++);
  DWANPORTB |= (1 << 3);  // Clock is active, SDA low
  DWANPORTB &= ~(1 << 4);
  for (int i = 0; i < 100; i++);
  DWANPORTB &= ~(1 << 3);
}

void i2c_stop() {
  DWANPORTB &= ~(1 << 4);
  for (int i = 0; i < 100; i++);
  DWANPORTB |= (1 << 3); // Reverse of i2c_start
  DWANPORTB |= (1 << 4);
}

void i2c_write_byte(uint8_t byte) {
  for (int i = 7; i >= 0; i--) {
    if ((byte >> i) & 1) {
      DWANPORTB |= (1 << 4);
    } else {
      DWANPORTB &= ~(1 << 4);
    }
    for (int k = 0; k < 100; k++);
    DWANPORTB |= (1 << 3);
    for (int k = 0; k < 100; k++);
    DWANPORTB &= ~(1 << 3);
  }

  DWANDIRB &= ~(1 << 4);
  for (int k = 0; k < 100; k++);
  DWANPORTB |= (1 << 3);
  bool ack = !(DWANPINB & (1 << 4));
  DWANPORTB &= ~(1 << 3);
  DWANDIRB |= (1 << 4);
}

uint8_t i2c_read_byte(bool send_ack) {
  uint8_t byte = 0;
  DWANDIRB &= ~(1 << 4);

  for (int i = 7; i >= 0; i--) {
    DWANPORTB |= (1 << 3);
    if (DWANPINB & (1 << 4)) {
      byte |= (1 << i);
    }
    DWANPORTB &= ~(1 << 3);
    for (int k = 0; k < 100; k++);
  }

  DWANDIRB |= (1 << 4);

  if (send_ack) {
    DWANPORTB &= ~(1 << 4);
  } else {
    DWANPORTB |= (1 << 4);
  }

  DWANPORTB |= (1 << 3);
  for (int k = 0; k < 100; k++);
  DWANPORTB &= ~(1 << 3);

  return byte;
}

void mpuraw_init(void) {
  i2c_init();
  i2c_start();
  i2c_write_byte((iic_ad << 1) | 0x00);
  i2c_write_byte(0x6B);  // Wake up MPU6050
  i2c_write_byte(0x00);  // Clear sleep mode
  i2c_stop();
}

int16_t mpuraw(uint8_t index) {
  uint8_t reg_start = 0x3B + (index - 1) * 2;  // Mapping index to the correct register address
  
  int16_t result = 0;
  uint8_t high, low;

  i2c_start();
  i2c_write_byte((iic_ad << 1) | 0x00);  // Send write request
  i2c_write_byte(reg_start);  // Request the corresponding register
  i2c_start();
  i2c_write_byte((iic_ad << 1) | 0x01);  // Send read request

  high = i2c_read_byte(true);
  low = i2c_read_byte(false);

  i2c_stop();

  result = (high << 8) | low;  // Combine high and low bytes to form the result
  return result;
}
