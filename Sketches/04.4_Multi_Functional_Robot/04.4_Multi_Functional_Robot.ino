/**********************************************************************
  Filename    : IR_Receiver_Robot.ino
  Product     : Freenove Robot for Raspberry Pi Pico (W)
  Auther      : www.freenove.com
  Modification: 2023/11/07
**********************************************************************/
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <IRremote.hpp>
#include <EEPROM.h>
#include "Freenove_Robot_For_Pico_W.h"
#include "Freenove_Robot_Emotion.h"
#include "Freenove_VK16K33_Lib.h"
#include "Freenove_Robot_WS2812.h"
#include "Bipedal_Robot.h"

// play music
#include "AudioFileSourceLittleFS.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2SNoDAC.h"
#include "AudioFileSourceID3.h"
#include "AudioOutputI2S.h"

//music
AudioGeneratorMP3 *mp3;
AudioFileSourceLittleFS *file;
AudioOutputI2SNoDAC *out;
AudioFileSourceID3 *id3;

#define LeftLeg 10
#define RightLeg 12
#define LeftFoot 11
#define RightFoot 13

#define IR_Pin 3  // Infrared receiving pin
#define ENABLE_LED_FEEDBACK true
#define DISABLE_LED_FEEDBACK false

int move_flag = 0;
extern int move_flag;
int YL;
int YR;
int RL;
int RR;

Bipedal_Robot Bipedal_Robot;
//int calibratePosition[4] = { -16, -24, -7, -32 };
int emotion_flag = 0;
int ws2812_flag = 0;
int music_flag = 0;
extern int music_flag;

void calib_homePos() {
  int servoPos[4];
  servoPos[0] = 90;
  servoPos[1] = 90;
  servoPos[2] = 90;
  servoPos[3] = 90;
  Bipedal_Robot._moveServos(500, servoPos);
  Bipedal_Robot.detachServos();
}

void setup() {
  Serial.begin(115200);  //Turn on the serial port monitor and set the baud rate to 115200
  delay(1500);
  WS2812_Setup();  //WS2812 initialization
  Buzzer_Setup();  //Initialize the buzzer
  Ultrasonic_Setup();
  EEPROM.begin(512);
  Bipedal_Robot.init(LeftLeg, RightLeg, LeftFoot, RightFoot, true);  //Set the servo pins and Buzzer pin
  YL = EEPROM.read(0);
  if (YL > 128) YL -= 256;
  YR = EEPROM.read(1);
  if (YR > 128) YR -= 256;
  RL = EEPROM.read(2);
  if (RL > 128) RL -= 256;
  RR = EEPROM.read(3);
  if (RR > 128) RR -= 256;
  Serial.println("YL: " + String(YL) + " YR: " + String(YR) + " RL: " + String(RL) + " RR: " + String(RR));
  Bipedal_Robot.setTrims(YL, YR, RL, RR);
  calib_homePos();
  Bipedal_Robot.saveTrimsOnEEPROM();
  EEPROM.commit();
  Bipedal_Robot.home();
  IrReceiver.begin(IR_Pin, DISABLE_LED_FEEDBACK);  // Start the receiver
  delay(100);
}

void loop() {
  if (IrReceiver.decode()) {
    unsigned long value = IrReceiver.decodedIRData.decodedRawData;
    handleControl(value);        // Handle the commands from remote control
    Serial.println(value, HEX);  // Print "old" raw data
    Serial.println();
    IrReceiver.resume();  // Enable receiving of the next value
    Serial.println("YL: " + String(YL) + " YR: " + String(YR) + " RL: " + String(RL) + " RR: " + String(RR));
  }
  Emotion_Detection();
  Emotion_Show(emotion_task_mode);  //Led matrix display function
  WS2812_Show(ws2812_task_mode);    //Car color lights display function
  Move(move_flag);
}

void setup1() {
  delay(2000);
  playmusic(2, 1);
  pinMode(6, OUTPUT);
  digitalWrite(6, LOW);
}
void loop1() {
  if (music_flag > 0) {
    playmusic(1, music_flag);
    music_flag = 0;
  } else {
    digitalWrite(6, LOW);
  }
}

void handleControl(unsigned long value) {
  // Handle the commands
  switch (value) {
    case 0xBF40FF00:  // Receive the number '+'
      move_flag = 1;
      break;
    case 0xE619FF00:  // Receive the number '-'
      move_flag = 2;
      break;
    case 0xF807FF00:  // Receive the number '|<<'
      move_flag = 3;
      break;
    case 0xF609FF00:  // Receive the number '>>|'
      move_flag = 4;
      break;
    case 0xEA15FF00:  // Receive the number '▶'
      move_flag = 0;
      break;
    case 0xE916FF00:  // Receive the number '0'
      music_flag = 1;
      break;
    case 0xF30CFF00:  // Receive the number '1'
      music_flag = 2;
      break;
    case 0xF708FF00:  // Receive the number '4'
      music_flag = 0;
      break;
    case 0xF20DFF00:  // Receive the number 'C'
      break;
    case 0xA15EFF00:  // Receive the number '3'
      move_flag = 5;
      break;
    case 0xA55AFF00:  // Receive the number '6'
      move_flag = 6;
      break;
    case 0xB54AFF00:  // Receive the number '9'
      break;
    case 0xBB44FF00:  // Receive the number 'TEST'
      Buzzer_Variable(2000, 100, 1);
      break;
    case 0xE718FF00:  // Receive the number '2'
      emotion_flag = emotion_flag + 1;
      if (emotion_flag > 7) {
        emotion_flag = 0;
      }
      Emotion_SetMode(emotion_flag);  //Display
      break;
    case 0xE31CFF00:  // Receive the number '5'
      emotion_flag = 0;
      Emotion_SetMode(emotion_flag);
      break;
    case 0xBD42FF00:  // Receive the number '7'
      ws2812_flag = ws2812_flag + 1;
      if (ws2812_flag >= 6) {
        ws2812_flag = 0;
      }
      WS2812_SetMode(ws2812_flag);
      break;
    case 0xAD52FF00:  // Receive the number '8'
      ws2812_flag = 0;
      WS2812_SetMode(ws2812_flag);
      break;
    case 0xFFFFFFFF:  // Remain unchanged
      break;
    default:
      break;
  }
}

void Move(int data) {
  // Handle the commands
  switch (data) {
    case 1:  // Receive the number '+'
      Bipedal_Robot.walk(2, 1500, 1);
      move_flag = 0;
      break;
    case 2:  // Receive the number '-'
      Bipedal_Robot.walk(2, 1500, -1);
      move_flag = 0;
      break;
    case 3:  // Receive the number '|<<'
      Bipedal_Robot.turn(2, 1500, 1);
      move_flag = 0;
      break;
    case 4:  // Receive the number '>>|'
      Bipedal_Robot.turn(2, 1500, -1);
      move_flag = 0;
      break;
    case 5:  // Receive the number '>>|'
      Ultrasonic_Avoid();
      break;
    case 6:  // Receive the number '>>|'
      dance();
      move_flag = 0;
      break;
    case 0:  // Receive the number '▶'
      Bipedal_Robot.home();
      break;
    default:
      move_flag = 0;
      Bipedal_Robot.home();
      break;
  }
}

void dance() {
  Bipedal_Robot.jitter(1, 1000, 40);
  Bipedal_Robot.home();
  Bipedal_Robot.moonwalker(1, 1200, 30, 1);
  Bipedal_Robot.home();
  Bipedal_Robot.ascendingTurn(1, 1000, 50);
  Bipedal_Robot.home();
  Bipedal_Robot.tiptoeSwing(1, 1000, 30);
  Bipedal_Robot.home();
  Bipedal_Robot.flapping(1, 1000, 40, 1);
  Bipedal_Robot.home();
  Bipedal_Robot.crusaito(1, 3000, 40, 1);
  Bipedal_Robot.home();
  Bipedal_Robot.shakeLeg(1, 1000, 1);
  Bipedal_Robot.home();
}

void playmusic(int playtimes, int song) {
  if (playtimes--) {
    if (song == 1) {
      file = new AudioFileSourceLittleFS("Hello.mp3");
    } else if (song == 2) {
      file = new AudioFileSourceLittleFS("Nicetomeetyou.mp3");
    } else if (song == 0) {
      file = new AudioFileSourceLittleFS("split.mp3");
    } else if (song == 3) {
      file = new AudioFileSourceLittleFS("goodbye.mp3");
    } else if (song == 4) {
      file = new AudioFileSourceLittleFS("hello_cn.mp3");
    }
    out = new AudioOutputI2SNoDAC(6, 26);
    out->SetGain(3.5);  //Volume Setup
    mp3 = new AudioGeneratorMP3();
    mp3->begin(file, out);
    if (mp3->isRunning() && song > 0) {
      if (!mp3->loop() && song > 0) {
        delete file;
        delete mp3;
        out->flush();
        out->stop();
      }
    } else {
      delete file;
      delete mp3;
      out->flush();
      out->stop();
      pinMode(6, OUTPUT);
      digitalWrite(6, LOW);
    }
  }
}
