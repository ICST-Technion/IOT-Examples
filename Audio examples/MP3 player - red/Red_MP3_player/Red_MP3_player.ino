
// Include required libraries:
#include <HardwareSerial.h>

// Define the Serial MP3 Player Module.
HardwareSerial MP3(2);  // Use UART2 for MP3 player communication
// GPIO17 ------------------------ MP3 TX
// GPIO16 ------------------------ MP3 RX

// Define the required MP3 Player Commands: based on command codes listed in manual - https://static1.squarespace.com/static/584d41b3f5e2310b396cd953/t/5c7c2f29104c7b336a2f8380/1551642412037/Serial+MP3+Player+A+v1.0+Manual.pdf
static byte start_byte = 0x7E;  // Start byte
static byte end_byte = 0xEF;    // End byte

static byte set_volume_CMD = 0x31;
static byte play_index_CMD = 0x41;
static byte play_filename_CMD = 0x42;
static int8_t select_SD_CMD[] = { 0x7e, 0x03, 0X35, 0x01, 0xef };
static int8_t reset_CMD[] = { 0x7e, 0x03, 0X35, 0x05, 0xef };

void setup() {
  // Initiate the serial monitor.
  Serial.begin(9600);
  // Initiate the Serial MP3 Player Module.
  MP3.begin(9600, SERIAL_8N1, 17, 16);
  delay(100);  //delay for stability
  if (reset_mp3() == true) {
    Serial.println("reset MP3 success");
  } else {
    Serial.println("reset MP3 fail");
    delay(10000);
  }
  select_SD_card();  //ALWAYS select SD card at beginning
  delay(1200);       //indexing the files on your SD card will take at least 1 second. let the operation finish in the background before trying to play a file.
}

void loop() {


  set_volume(2);
  play_filename(1, 1);
  delay(3000);
  set_volume(15);
  play_filename(1, 2);
  delay(3000);
  set_volume(25);
  play_filename(1, 3);
  delay(3000);
}

bool reset_mp3() {
  Serial.println("MP3 RESET");
  MP3.flush();  //delete all data in serial2 buffer for MP3
  int len = 5;
  for (int i = 0; i < len; i++) {
    MP3.write(reset_CMD[i]);
  }
  delay(50);              //give the mp3 player time to reset and return message
  if (MP3.available()) {  //check if there is a response from the board (not reading the response itself)
    return true;
  } else {
    return false;
  }
}

void select_SD_card() {
  Serial.println("MP3_select_SD_card");
  int len = 4;
  for (int i = 0; i <= len; i++) {
    MP3.write(select_SD_CMD[i]);
  }
}

void set_volume(byte volume) {
  Serial.print("set volume= ");
  Serial.print(volume);
  Serial.println(" of 30");
  MP3.write(start_byte);
  byte msg_len = 0x03;
  MP3.write(msg_len);
  MP3.write(set_volume_CMD);
  MP3.write(volume);
  MP3.write(end_byte);
  delay(20);  //required for stability
}

// void play_index(byte index) { //play_index plays the file according to the order (without directory number). in some cases the order is different than the file number. 
//   Serial.print("playing file index => ");
//   Serial.println(index);
//   MP3.write(start_byte);
//   byte msg_len = 0x04;
//   MP3.write(msg_len);
//   MP3.write(play_index_CMD);
//   byte zero = 0x00;
//   MP3.write(zero);
//   MP3.write(index);
//   MP3.write(end_byte);
//   delay(20);  //required for stability
// }

void play_filename(int8_t directory, int8_t file) {
  Serial.print("playing directory ");
  Serial.print(directory);
  Serial.print(" file number ");
  Serial.println(file);
  MP3.write(start_byte);
  byte msg_len = 0x04;
  MP3.write(msg_len);
  MP3.write(play_filename_CMD);
  byte dir_ind = directory;
  MP3.write(dir_ind);
  byte file_ind = file;
  MP3.write(file_ind);
  MP3.write(end_byte);
  delay(20);  //required for stability
}
