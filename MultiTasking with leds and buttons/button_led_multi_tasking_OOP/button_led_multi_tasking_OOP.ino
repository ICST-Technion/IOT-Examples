// this code demonstrates "multi-tasking" in ESP32 using timers: millis()
// the blink rate of the LED can be changed any time using a global variable
// the button function can store the number of presses, length of each press (for detecting long press, ignoring too short presses)
// all without using delay function
//
// the code counts button presses but will ignore very short presses
// you can also print out the length of last press or the current press duration. 

#include <Adafruit_NeoPixel.h>
#include "BlinkingLED.h"
#include "ButtonHandler.h"

#define PIN        26 //pin for neopixels
#define NUMPIXELS 2 //number of neopixels

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

BlinkingLED led0(pixels, 0, 100);
BlinkingLED led1(pixels, 1, 300);
ButtonHandler button0(13);
ButtonHandler button1(14);

void setup() {
  Serial.begin(9600);
  pixels.begin();
  pixels.clear(); 
  button0.begin();
  button1.begin();
}

void loop() {
if (button0.handle()) {print_debug_data(0, button0.last_press_duration() , button0.press_counter());} //handle_button returns true if press is detected. 
if (button1.handle()) {print_debug_data(1, button1.last_press_duration() , button1.press_counter());} //handle_button returns true if press is detected. 
led0.update();
led1.update();
}

void print_debug_data(byte index, int duration, int counter)
{
        Serial.print("button ");
        Serial.print(index);
        Serial.print(" pressed!    duration= ");
        Serial.print(duration);
        Serial.print(" counter=");
        Serial.println(counter);
}
 