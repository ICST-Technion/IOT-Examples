// this code demonstrates "multi-tasking" in ESP32 using timers: millis()
// the blink rate of the LED can be changed any time using a global variable
// the button function can store the number of presses, length of each press (for detecting long press, ignoring too short presses)
// all without using delay function
//
// the code counts button presses but will ignore very short presses
// you can also print out the length of last press or the current press duration.

//************************************************************************************************
//LIVE SIMULATOR WITH THIS CODE IS AVAILABLE AT - https://wokwi.com/projects/373921539051463681
//************************************************************************************************

#include <Adafruit_NeoPixel.h>

#define PIN        26 //pin for neopixels
#define NUMPIXELS 2 //number of neopixels
#define MIN_PRESS_TIME 40 //minimum duration in milliseconds to consider a button press

//global variables for leds
bool currentLedState [] = { false , false };
unsigned long previousBlinkTime [] = { 0 , 0 };   // last time led state was changed
unsigned int blinkIntervals [] = { 300 , 150 };   // last time led state was changed

//global variables for buttons
const byte button_pins[] = {13, 14};
bool currentButtonStates [] = {true, true};
bool previousButtonStates [] = {true, true}; //
bool previousButtonState = HIGH; // will store last time button was updated
unsigned int buttonPress_counters [] = { 0 , 0 }; //number of presses
unsigned long buttonPress_last_times [] = { 0 , 0 };   // last time button was pressed
unsigned long last_press_duration = 0; //to allow accessing last press duration as global

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(9600);
  pinMode(button_pins[0], INPUT_PULLUP);   // set the digital pin as an input with pullup resistor (important)
  pinMode(button_pins[1], INPUT_PULLUP);   // set the digital pin as an input with pullup resistor (important)
  pixels.begin();
  pixels.clear();
}

void loop() {
  if (handle_button(0)) {
    print_debug_data(0); //handle_button returns true if press is detected.
  }
  if (handle_button(1)) {
    print_debug_data(1); //handle_button returns true if press is detected.
  }
  handle_led_blinker(0);
  handle_led_blinker(1);
}


void handle_led_blinker(byte led_index)
{
  // check to see if it's time to blink the LED; that is, if the difference
  // between the current time and last time you blinked the LED is bigger than
  // the interval at which you want to blink the LED.
  unsigned long currentMillis = millis();
  if (currentMillis - previousBlinkTime[led_index] >= blinkIntervals[led_index]) {
    if (currentLedState[led_index] == false)  // if the LED is off turn it on and vice-versa:
    {
      pixels.setPixelColor(led_index, pixels.Color(0, 150, 0)); //turn on the led (green color)
      currentLedState[led_index] = true;
    }
    else
    {
      pixels.setPixelColor(led_index, pixels.Color(0, 0, 0)); //turn off the led
      currentLedState[led_index] = false;
    }
    pixels.show();   // Send the updated pixel colors to the hardware.
    previousBlinkTime[led_index] = currentMillis; // save the last time you blinked the LED
  }
}


bool handle_button(byte button_index) { //handle a button
  bool previousButtonState = previousButtonStates[button_index];
  bool currentButtonState = digitalRead(button_pins[button_index]);  // check button state's change

  if (currentButtonState != previousButtonState)  //if change is detected
  {
    previousButtonStates[button_index] = currentButtonState;       // save the last state of button
    if (currentButtonState == LOW && previousButtonState == HIGH)  //type of change - from released to pressed
    {
      buttonPress_last_times[button_index] = millis();
    } else {
      last_press_duration = millis() - buttonPress_last_times[button_index];
      if (last_press_duration > MIN_PRESS_TIME) {
        buttonPress_counters[button_index] = buttonPress_counters[button_index] + 1;
        return (true);
      }
    }
  }
  return (false);
}

void print_debug_data(byte index)
{
  Serial.print("button ");
  Serial.print(index);
  Serial.print(" pressed!    duration= ");
  Serial.print(last_press_duration);
  Serial.print(" counter=");
  Serial.println(buttonPress_counters[index]);
}
