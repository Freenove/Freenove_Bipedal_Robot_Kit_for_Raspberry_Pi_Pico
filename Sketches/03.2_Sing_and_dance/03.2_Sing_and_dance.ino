/**********************************************************************
  Product     : Freenove Robot for Raspberry Pi Pico (W)
  Description : Sing and dance.
  Auther      : www.freenove.com
  Modification: 2023/11/07
**********************************************************************/
#include <Arduino.h>
#include <EEPROM.h>
#include "Bipedal_Robot.h"
#include "AudioFileSourceLittleFS.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2SNoDAC.h"
#include "AudioFileSourceID3.h"
#include "AudioOutputI2S.h"

Bipedal_Robot Bipedal_Robot;
AudioGeneratorMP3 *mp3;
AudioFileSourceLittleFS *file;
AudioOutputI2SNoDAC *out;
AudioFileSourceID3 *id3;

#define LeftLeg 10
#define RightLeg 12
#define LeftFoot 11
#define RightFoot 13

int calibratePosition[4] = { -17, -10, -15, 12 };

void playmusic(int playtimes, int song) {
  if (playtimes--) {
    if (song == 1) {
      file = new AudioFileSourceLittleFS("say-goodbye2.mp3");
    } else if (song == 2) {
      file = new AudioFileSourceLittleFS("Hello.mp3");
    } else if (song == 3) {
      file = new AudioFileSourceLittleFS("onfoot.mp3");
    } else if (song == 4) {
      file = new AudioFileSourceLittleFS("1.mp3");
    } else if (song == 5) {
      file = new AudioFileSourceLittleFS("onfoot.mp3");
    }
    //    else if (song == 0) break;
    else if (song == 0) {
      file = new AudioFileSourceLittleFS("split.mp3");
    }

    // file = new AudioFileSourceLittleFS("Hello.mp3");
    out = new AudioOutputI2SNoDAC(6, 26);
    out->SetGain(4.0);  //Volume Setup
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
      Serial.printf("play music..\n");
      pinMode(6, OUTPUT);
      digitalWrite(6, LOW);
    }
  }
}

void dance() {
  delay(1000);
  Bipedal_Robot.jitter(10, 1000, 40);
  Bipedal_Robot.home();
  Bipedal_Robot.moonwalker(2, 1200, 30, 1);
  Bipedal_Robot.home();
  Bipedal_Robot.ascendingTurn(2, 1000, 50);
  Bipedal_Robot.home();
  Bipedal_Robot.tiptoeSwing(2, 1000, 30);
  Bipedal_Robot.home();
  Bipedal_Robot.flapping(2, 1000, 40, 1);
  Bipedal_Robot.home();
  Bipedal_Robot.crusaito(2, 2000, 40, 1);
  Bipedal_Robot.home();
  Bipedal_Robot.shakeLeg(2, 1000, 1);
  Bipedal_Robot.home();
}

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
  Serial.begin(115200);
  EEPROM.begin(512);                                                      //Initialize the ultrasonic module
  Bipedal_Robot.init(LeftLeg, RightLeg, LeftFoot, RightFoot, true);  //Set the servo pins and Buzzer pin
  //Bipedal_Robot.setTrims(calibratePosition[0], calibratePosition[1], calibratePosition[2], calibratePosition[3]);
  calib_homePos();
  Bipedal_Robot.saveTrimsOnEEPROM();
  EEPROM.commit();
  Bipedal_Robot.home();
  delay(50);
}
void loop() {
  dance();
}

void setup1() {
  playmusic(1, 2);
  pinMode(6, OUTPUT);
  digitalWrite(6, LOW);
}
void loop1() {
  playmusic(1,4);
}