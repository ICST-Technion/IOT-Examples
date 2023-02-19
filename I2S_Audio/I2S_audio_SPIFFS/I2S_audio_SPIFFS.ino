//**********************************************************************************************************
//* based on audioI2S-- I2S audiodecoder for ESP32, https://github.com/schreibfaul1/ESP32-audioI2S/wiki    *
//**********************************************************************************************************

//#include "Arduino.h" //required for PlatformIO
#include "Audio.h"
#include "FS.h"

// Digital I/O used for I2S DAC amplifier
#define I2S_DOUT      25
#define I2S_BCLK      27
#define I2S_LRC       26

Audio audio;

void setup() {
   
  Serial.begin(115200);

    if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
  audio.setVolume(20); // range 0...21 - This is not amplifier gain, but controlling the level of output amplitude. 

  audio.connecttoFS(SPIFFS, "123_u8.wav"); // a file with the proper name must be placed in /data folder and uploaded using "ESP32 sketch upload" plugin

  audio.setFileLoop(true); //this causes the file to play in an endless loop
}

void loop()
{
  audio.loop();
  if (Serial.available()) { // put streamURL in serial monitor
    audio.stopSong();
    Serial.println("audio stopped");
    log_i("free heap=%i", ESP.getFreeHeap());

  }
}
