//**********************************************************************************************************
//* based on audioI2S-- I2S audiodecoder for ESP32, https://github.com/schreibfaul1/ESP32-audioI2S/wiki    *
//**********************************************************************************************************

//#include "Arduino.h" //required for PlatformIO

#include "Audio.h"
#include "SPI.h"
#include "SD.h"
#include "FS.h"

// Digital I/O used
#define SD_CS          5
#define SPI_MOSI      23
#define SPI_MISO      19
#define SPI_SCK       18
#define I2S_DOUT      25
#define I2S_BCLK      27
#define I2S_LRC       26

Audio audio;

void setup() {
  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, HIGH);
  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
  SPI.setFrequency(1000000);
  Serial.begin(115200);
  SD.begin(SD_CS);
  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
  audio.setVolume(12); // range 0...21 - This is not amplifier gain, but controlling the level of output amplitude. 

  audio.connecttoFS(SD, "123_u8.wav"); // a file with the proper name must be placed in root folder of SD card (formatted FAT32, file name max 8 chars no spaces)
  
  audio.setFileLoop(true); //this causes the file to play in an endless loop
}

void loop()
{
  audio.loop();
 
  if (Serial.available()) { // if any string is sent via serial port
    audio.stopSong();
    Serial.println("audio stopped");
    log_i("free heap=%i", ESP.getFreeHeap()); //available RAM
    Serial.flush();
  }
}
