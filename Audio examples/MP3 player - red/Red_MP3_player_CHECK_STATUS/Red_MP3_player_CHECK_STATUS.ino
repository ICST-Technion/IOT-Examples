//Demo for Serial MP3 Player A module
//by Open-Smart Team and Catalex Team
//catalex_inc@163.com
//Board:  Arduino UNO R3
//IDE:	  Arduino-1.6.4
//Store: http://www.aliexpress.com/store/1199788
//      http://dx.com
//Function:  In the process that the Rotation Angle Sensor is rotated from
//               the 'Min' side to the 'Max' side, the volume is gradually greater.
//               If you touch the Touch Sensor,it will play or pause.

/* Wiring Guide */
//Serial MP3 Player A     |  Arduino UNO R3//
//              RX              |   7
//              TX              |   8
//              VCC            |   5V
//              GND            |   GND
//-------------------------------------
//     Touch Sensor         |  Arduino UNO R3//
//              SIG             |   2
//              VCC            |   5V
//              GND            |   GND
//-------------------------------------
// Rotary Angle Sensor   |  Arduino UNO R3//
//              SIG             |   A0
//              VCC            |   5V
//              GND            |   GND
// Include required libraries:
#include <HardwareSerial.h>

HardwareSerial myMP3(2);

static int8_t Send_buf[6] = { 0 };
char receivedData[20];
int dataIndex = 0;
/************Command byte**************************/
/*basic commands*/
#define CMD_PLAY 0X01
#define CMD_PAUSE 0X02
#define CMD_NEXT_SONG 0X03
#define CMD_PREV_SONG 0X04
#define CMD_VOLUME_UP 0X05
#define CMD_VOLUME_DOWN 0X06
#define CMD_FORWARD 0X0A  // >>
#define CMD_REWIND 0X0B   // <<
#define CMD_STOP 0X0E
#define CMD_STOP_INJECT 0X0F  //stop interruptting with a song, just stop the interlude

/*5 bytes commands*/
#define CMD_SEL_DEV 0X35
#define DEV_TF 0X01
#define CMD_IC_MODE 0X35
#define CMD_SLEEP 0X03
#define CMD_WAKE_UP 0X02
#define CMD_RESET 0X05

/*6 bytes commands*/
#define CMD_PLAY_W_INDEX 0X41
#define CMD_PLAY_FILE_NAME 0X42
#define CMD_INJECT_W_INDEX 0X43

/*Special commands*/
#define CMD_SET_VOLUME 0X31
#define CMD_PLAY_W_VOL 0X31

#define CMD_SET_PLAY_MODE 0X33
#define ALL_CYCLE 0X00
#define SINGLE_CYCLE 0X01

#define CMD_PLAY_COMBINE 0X45  //can play combination up to 15 songs

#define CMD_STATUS 0X10

void sendCommand(int8_t command, int16_t dat = 0);

void setup() {
  Serial.begin(9600);
  myMP3.begin(9600, SERIAL_8N1, 17, 16);
  sendCommand(CMD_RESET);
  delay(1500);
  sendCommand(CMD_SEL_DEV, DEV_TF);
  delay(500);
  setVolume(8);
  sendCommand(CMD_PLAY_FILE_NAME, 0x0103);
  // sendCommand(CMD_STATUS);
}

void loop() {
  if (check_MP3_status() == 1) {
    sendCommand(CMD_PLAY_FILE_NAME, 0x0101);
  }
  // sendCommand(CMD_STATUS);
  delay(100);
}



int check_MP3_status()
{
    while (myMP3.available()) { //should not be a while loop, but a if loop. but MP3 sends very little data, so it should be fine
    // get the new byte:
    char player_status = myMP3.read();
    receivedData[dataIndex] = player_status;
    Serial.print(player_status, HEX);
    delay(1);                               //At 9600 baud , each char takes about 0.9ms
    if (receivedData[dataIndex] == 0xEF) {  //reached end of reply
      Serial.print(" LEN=");
      Serial.println(dataIndex);
      if (receivedData[dataIndex - 3] == 0x3D && receivedData[dataIndex - 4] == 0x04) {  //7E.4.3D.(folder_byte).(file_byte).EF  - stopped msg - sent twice by MP3
        Serial.print(" STOP MSG #");
        if (dataIndex > 5) { //if the message length is 5, then this is the first time "stop" is sent. ignore it and wait for the second time.
          dataIndex = 0;
          Serial.println(" 2"); 
          return(1);
        }
        else {
          Serial.println(" 1");
          return(0);}
        
      } else if (receivedData[dataIndex - 1] == 0x00 && receivedData[dataIndex - 2] == 0x31) {  //7E20EF7E3101EF - status reply - currently playing
        Serial.println(" STATUS REPLY - IDLE");
        dataIndex = 0;
        return(2);
      } else if (receivedData[dataIndex - 1] == 0x01 && receivedData[dataIndex - 2] == 0x31) {  //7E20EF7E3101EF - status reply - currently playing
        Serial.println(" STATUS REPLY - PLAYING");
        dataIndex = 0;
        return(3);
      }
      dataIndex = 0;
    } else {
      dataIndex++;
      Serial.print(".");
    }
  }
  return(0);
}


void setVolume(int8_t vol) {
  mp3_5bytes(CMD_SET_VOLUME, vol);
}
void playWithVolume(int16_t dat) {
  mp3_6bytes(CMD_PLAY_W_VOL, dat);
}

/*cycle play with an index*/
void cyclePlay(int16_t index) {
  mp3_6bytes(CMD_SET_PLAY_MODE, index);
}

void setCyleMode(int8_t AllSingle) {
  mp3_5bytes(CMD_SET_PLAY_MODE, AllSingle);
}


void playCombine(int8_t song[][2], int8_t number) {
  if (number > 15) return;  //number of songs combined can not be more than 15
  uint8_t nbytes;           //the number of bytes of the command with starting byte and ending byte
  nbytes = 2 * number + 4;
  int8_t Send_buf[nbytes];
  Send_buf[0] = 0x7e;        //starting byte
  Send_buf[1] = nbytes - 2;  //the number of bytes of the command without starting byte and ending byte
  Send_buf[2] = CMD_PLAY_COMBINE;
  for (uint8_t i = 0; i < number; i++)  //
  {
    Send_buf[i * 2 + 3] = song[i][0];
    Send_buf[i * 2 + 4] = song[i][1];
  }
  Send_buf[nbytes - 1] = 0xef;
  sendBytes(nbytes);
}


void sendCommand(int8_t command, int16_t dat) {
  delay(20);
  if ((command == CMD_PLAY_W_VOL) || (command == CMD_SET_PLAY_MODE) || (command == CMD_PLAY_COMBINE))
    return;
  else if (command <= 0x10) {
    mp3Basic(command);
  } else if (command < 0x40) {
    mp3_5bytes(command, dat);
  } else if (command < 0x50) {
    mp3_6bytes(command, dat);
  } else return;
}

void mp3Basic(int8_t command) {
  Send_buf[0] = 0x7e;  //starting byte
  Send_buf[1] = 0x02;  //the number of bytes of the command without starting byte and ending byte
  Send_buf[2] = command;
  Send_buf[3] = 0xef;  //
  sendBytes(4);
}
void mp3_5bytes(int8_t command, uint8_t dat) {
  Send_buf[0] = 0x7e;  //starting byte
  Send_buf[1] = 0x03;  //the number of bytes of the command without starting byte and ending byte
  Send_buf[2] = command;
  Send_buf[3] = dat;   //
  Send_buf[4] = 0xef;  //
  sendBytes(5);
}
void mp3_6bytes(int8_t command, int16_t dat) {
  Send_buf[0] = 0x7e;  //starting byte
  Send_buf[1] = 0x04;  //the number of bytes of the command without starting byte and ending byte
  Send_buf[2] = command;
  Send_buf[3] = (int8_t)(dat >> 8);  //datah
  Send_buf[4] = (int8_t)(dat);       //datal
  Send_buf[5] = 0xef;                //
  sendBytes(6);
}
void sendBytes(uint8_t nbytes) {
  for (uint8_t i = 0; i < nbytes; i++)  //
  {
    myMP3.write(Send_buf[i]);
  }
}
