#include <Adafruit_NeoPixel.h>

#define PIN        17 // On Trinket or Gemma, suggest changing this to 1

#define NUMPIXELS 6 // using over 8 pixels with ESP32 requires external power supply 

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

#define DELAYVAL 5 // Time (in milliseconds) 

uint32_t rgbcolor=0;
uint32_t rgbcolor_corrected=0;

void setup() {
  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
    pixels.clear();    // turn all LEDs off
      pixels.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)
  pixels.show();     // Initialize all pixels
  Serial.begin(9600);
  Serial.println("setup complete");
  delay(500);
}

void loop() {

  for(int hue=0; hue<65536; hue+=10) { // For each pixel...
    rgbcolor = pixels.ColorHSV(hue);
    rgbcolor_corrected=pixels.gamma32(rgbcolor);
    
    Serial.println(hue);
    
    pixels.fill(rgbcolor,0,3); // this will fill neopixels 0-2 
    pixels.fill(rgbcolor_corrected,3,5); // this will fill neopixels 3-5
    
    pixels.show();   // Send the updated pixel colors to the hardware.
    delay(DELAYVAL); // Pause before next pass through loop
  }

  
}
