#include <Adafruit_NeoPixel.h>
#define PIN        17 // On Trinket or Gemma, suggest changing this to 1
#define NUMPIXELS 6 // using over 8 pixels with ESP32 requires external power supply 

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

#define DELAYVAL 5 // Time (in milliseconds) 
uint32_t rgbcolor = 0;
int led_index = 0;
uint32_t led_hue = 0;

void setup() {
  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.clear();    // turn all LEDs off
  pixels.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)
  pixels.show();     // Initialize all pixels
  Serial.begin(9600);
  Serial.println("\n setup complete. input [pixel index 1->5 ,pixel hue 0->15] and press ENTER");
  delay(100);
}

void loop() {
  if ( Serial.available())
  {
    led_index = Serial.parseInt();
    led_hue = Serial.parseInt();
    if (led_index > 0) //there are no checks on the strings for simplicity. this causes zeros to be read randomly
    {
      Serial.print("LED index="); Serial.print(led_index);
      led_hue *= 4096; //color input will be in range 0-16 for simplicity, and rescaled to 0-65536
      Serial.print(" LED hue="); Serial.println(led_hue);
      rgbcolor = pixels.gamma32(pixels.ColorHSV(led_hue));
      pixels.setPixelColor(led_index, rgbcolor);
      pixels.show();   // Send the updated pixel colors to the hardware.
      delay(10);
    }
  }


}
