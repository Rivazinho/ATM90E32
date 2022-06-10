#define ESP32

#include <Wire.h>

void setup() {
Wire.begin(8);
Wire.onRequest(requestEvent);
}

void loop() {
delay(100);
}

byte datos = 0;

void requestEvent () {
  Wire.write(datos);
  datos++;
}
