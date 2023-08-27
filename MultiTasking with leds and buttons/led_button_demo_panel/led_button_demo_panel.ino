// this code demonstrates "multi-tasking" in ESP32 using timers: millis()
// the blink rate of the LED can be changed any time using a global variable
// the button function can store the number of presses, length of each press (for detecting long press, ignoring too short presses)
// all without using delay function
//
// the code counts button presses but will ignore very short presses
// you can also print out the length of last press or the current press duration.

#include <Adafruit_NeoPixel.h>

#define PIN 12        //pin for neopixels
#define NUMPIXELS 32  //number of neopixels
#define MIN_PRESS_TIME 40
#define ring_index 4
#define chain_index 5
#define analog_index 6

//global variables for leds
bool currentLedState[] = { false, false, false, false, false };
unsigned long previousBlinkTime[] = { 0, 0, 0, 0, 0, 0 ,0 };          // last time led state was changed
unsigned int blinkIntervals[] = { 500, 250, 500, 250, 50, 1000 ,50 };  // intervals
byte color_pickers[] = { 0, 1, 0, 1, 0, 1, 9 };
byte ring_current_led = 0;
byte chain_current_led = 0;
byte analog_fade_value = 0;
byte analog_color_pins[] = { 9, 10, 11 };

//global variables for buttons
const byte button_pins[] = { 2, 3, 4, 5, 8 };
bool currentButtonStates[] = { true, true, true, true, true };
bool currentButtonState = HIGH;
bool previousButtonStates[] = { true, true, true, true, true };
bool previousButtonState = HIGH;  // will store last time button was updated
unsigned int buttonPress_counters[] = { 0, 0, 0, 0, 0 };
unsigned long buttonPress_last_times[] = { 0, 0, 0, 0, 0 };  // last time button was pressed

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_RGB + NEO_KHZ800);


uint32_t black = pixels.Color(0, 0, 0);        //black
uint32_t white = pixels.Color(255, 255, 255);  //white
uint32_t red = pixels.Color(50, 0, 0);         //red
uint32_t orange = pixels.Color(255, 135, 0);   //orange
uint32_t yellow = pixels.Color(255, 255, 0);   //yellow
uint32_t green = pixels.Color(0, 50, 0);       //green
uint32_t blue = pixels.Color(0, 0, 65);        //blue
uint32_t cyan = pixels.Color(0, 255, 255);     //cyan
uint32_t purple = pixels.Color(150, 0, 150);   //purple
uint32_t pink = pixels.Color(150, 99, 108);    //pink

uint32_t colorarray[] = { orange, pink, yellow, cyan, white, purple };  // This is the color array.  You can add more colors if desired.

uint32_t colorarray_ring[] = { blue, green, red };

void setup() {
  Serial.begin(9600);
  pinMode(button_pins[0], INPUT_PULLUP);  // set the digital pin as an input:
  pinMode(button_pins[1], INPUT_PULLUP);  // set the digital pin as an input:
  pinMode(button_pins[2], INPUT_PULLUP);  // set the digital pin as an input:
  pinMode(button_pins[3], INPUT_PULLUP);  // set the digital pin as an input:
  pinMode(button_pins[4], INPUT_PULLUP);  // set the digital pin as an input:
  pinMode(6, INPUT_PULLUP);               // set the digital pin as an input:
  pinMode(7, INPUT_PULLUP);               // set the digital pin as an input:
  pinMode(analog_color_pins[0], OUTPUT);
  pinMode(analog_color_pins[1], OUTPUT);
  pinMode(analog_color_pins[2], OUTPUT);


  pixels.begin();
  pixels.clear();
}

void loop() {

  for (int i = 0; i < 4; i++) {
    if (handle_button(i)) {  //handle_button returns true if press was detected
      color_pickers[i]++;    //change led color for the blinking LEDs from list of 7 colors
      if (color_pickers[i] == 7) { color_pickers[i] = 0; }
    }
  }

  if (handle_button(4)) { //button to cycle between 3 colors RGB for the ring
    color_pickers[4]++;
    if (color_pickers[4] == 3) { color_pickers[4] = 0; }
  }

  handle_leds(0);
  handle_leds(1);
  handle_leds(2);
  handle_leds(3);
  handle_ring();
  handle_chain();
  handle_analog_fade();

  if (digitalRead(6)) { //handle 2-mode rocker switches
    blinkIntervals[4] = 50;

  } else {
    blinkIntervals[4] = 150;
  }

  if (digitalRead(7)) {//handle 2-mode rocker switches
    blinkIntervals[5] = 250;

  } else {
    blinkIntervals[5] = 1000;
  }
}



void handle_leds(byte led_index) { //handles blinking leds 

  unsigned long currentMillis = millis();
  if (currentMillis - previousBlinkTime[led_index] >= blinkIntervals[led_index]) {

    if (currentLedState[led_index] == false)  // if the LED is off turn it on and vice-versa:
    {

      pixels.setPixelColor(led_index, colorarray[color_pickers[led_index]]);  //turn on the led (green color)
      currentLedState[led_index] = true;
    } else {
      pixels.setPixelColor(led_index, black);  //turn off the led
      currentLedState[led_index] = false;
    }
    pixels.show();                                 // Send the updated pixel colors to the hardware.
    previousBlinkTime[led_index] = currentMillis;  // save the last time you blinked the LED
  }
}

void handle_ring() {  //handles 16 led ring chaser 
  unsigned long currentMillis = millis();
  if (currentMillis - previousBlinkTime[ring_index] >= blinkIntervals[ring_index]) {
    pixels.setPixelColor(ring_current_led + 8, colorarray_ring[color_pickers[ring_index]]);
    if (ring_current_led == 0) {
      pixels.setPixelColor(15 + 8, black);
    } else {
      pixels.setPixelColor(ring_current_led + 7, black);
    }

    ring_current_led++;
    if (ring_current_led == 16) { ring_current_led = 0; }
    pixels.show();                                  // Send the updated pixel colors to the hardware.
    previousBlinkTime[ring_index] = currentMillis;  // save the last time you blinked the LED
  }
}

void handle_chain() { // handle 4 led chaser
  unsigned long currentMillis = millis();
  if (currentMillis - previousBlinkTime[chain_index] >= blinkIntervals[chain_index]) {
    pixels.setPixelColor(chain_current_led + 4, colorarray_ring[color_pickers[chain_index]]);
    if (chain_current_led == 0) {
      pixels.setPixelColor(4 + 3, black);
    } else {
      pixels.setPixelColor(chain_current_led + 3, black);
    }
    chain_current_led++;
    if (chain_current_led == 4) { chain_current_led = 0; }
    color_pickers[chain_index]++;
    if (color_pickers[chain_index] == 3) { color_pickers[chain_index] = 0; }
    pixels.show();                                   // Send the updated pixel colors to the hardware.
    previousBlinkTime[chain_index] = currentMillis;  // save the last time you blinked the LED
  }
}

void handle_analog_fade() { //handle analog RGB LED cyclic fading
  unsigned long currentMillis = millis();
  if (currentMillis - previousBlinkTime[analog_index] >= blinkIntervals[analog_index]) {
    previousBlinkTime[analog_index] = currentMillis;    
    analog_fade_value += 1;
    if (analog_fade_value >= 100) {
      analog_fade_value = 0;
      analogWrite(color_pickers[analog_index], 0);
      color_pickers[analog_index]++;
      if (color_pickers[analog_index] == 12) { color_pickers[analog_index] = 9; }
    }
    else
    {
      analogWrite(color_pickers[analog_index], analog_fade_value);
    }
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
      unsigned long last_press_duration = millis() - buttonPress_last_times[button_index];
      if (last_press_duration > MIN_PRESS_TIME) {
        buttonPress_counters[button_index] = buttonPress_counters[button_index] + 1;
        Serial.print("press duration= ");
        Serial.println(last_press_duration);
        Serial.print(buttonPress_counters[0]);
        Serial.print(" , ");
        Serial.print(buttonPress_counters[1]);
        Serial.println();
        return (true);
      }
    }
  }
  return (false);
}
