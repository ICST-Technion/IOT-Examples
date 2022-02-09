#define LED_PIN 13
#define BLINK_RATE 100

#include "blinker.h"

void setup() {
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  blinker();
}
