#include <MPURaw.h>

void setup() {
  Serial.begin(9600);
  mpuraw_init();                  // arduino(nano and uno) comapatible only use 11 pin(scl), 12 pin(sda) 
}

void loop() {
  for (int i = 1; i <= 6; i++) {
    int16_t data = mpuraw(i);                       //mpuraw(1)=accx, mpuraw(2)=accy,mpuraw(3)=accz,mpuraw(4)=gyrox,mpuraw(5)=gyroy,mpuraw(6)=gyroz
    Serial.print("Data from mpu ");
    Serial.print(i);
    Serial.print(": ");
    Serial.println(data);
  }
  delay(1000);
}
