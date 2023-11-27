/*
  outputCW.ino

  The ESP32 contains two 8-bit DAC output channels.
  DAC channel 1 is GPIO25 (Pin 25) and DAC channel 2 is GPIO26 (Pin 26).

  This sketch generates a ~1000Hz sinus signal with changing amplitude on 
  DAC channel 1 (Pin 25) using the integrated cosine waveform (CW) generator.

  Last updated 2022-06-08, ThJ <yellobyte@bluewin.ch>
*/

#include <Arduino.h>
#include "DacESP32.h"
int freqencies[] = { 523, 587, 659, 698, 784, 880, 988, 1046 };
DacESP32 dac1(GPIO_NUM_25);
int vol = 0;

void setup() {
  Serial.begin(115200);

  Serial.println();
  Serial.print("Sketch started. Sinus signal on GPIO (Pin) number: ");
  Serial.println(GPIO_NUM_25);
}

void loop() {
  for (uint8_t i = 0; i < 8; i++) {
    dac1.outputCW(freqencies[i]);
    delay(500);
  }
  vol++;
  Serial.println(vol);
  if (vol == 0) {
    dac1.setCwScale(DAC_CW_SCALE_1);
  } else if (vol == 1) {
    dac1.setCwScale(DAC_CW_SCALE_2);
  } else if (vol == 2) {
    dac1.setCwScale(DAC_CW_SCALE_4);
  } else if (vol == 3) {
    dac1.setCwScale(DAC_CW_SCALE_8);
    vol = -1;
  }

}
