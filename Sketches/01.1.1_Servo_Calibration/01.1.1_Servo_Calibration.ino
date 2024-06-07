/**********************************************************************
  Product     : Freenove Robot for Raspberry Pi Pico (W)
  Description : Calibration robot.
  Auther      : www.freenove.com
  Modification: 2023/11/07
**********************************************************************/
/*Check that the servos are properly connected to the corresponding 
interface before uploading the code. */
/************************************
           ---     ---
         --------------- 
        |     O   O     |
        |---------------|
YR 12==>|               | <== YL 10
         --------------- 
            ||     ||
            ||     ||
RR 13==>  -----   ------  <== RL 11
         |-----   ------|
************************************/
#include <Arduino.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <EEPROM.h>
#include "Bipedal_Robot.h"
Bipedal_Robot Bipedal_Robot;

#define LeftLeg 10
#define RightLeg 12
#define LeftFoot 11
#define RightFoot 13

int calibratePosition[4] = { 0, 0, 0, 0 };

// Used to save the servo offset, in radians
int YL;
int YR;
int RL;
int RR;
char charRead;
void setup() {
  EEPROM.begin(512);
  Serial.begin(9600);
  for (int i = 0; i < 4; i++) {
    EEPROM.write(i, calibratePosition[i]);
  }
  Bipedal_Robot.init(LeftLeg, RightLeg, LeftFoot, RightFoot, true);  //Set the servo pins
  YL = EEPROM.read(0);
  if (YL > 128) YL -= 256;
  YR = EEPROM.read(1);
  if (YR > 128) YR -= 256;
  RL = EEPROM.read(2);
  if (RL > 128) RL -= 256;
  RR = EEPROM.read(3);
  if (RR > 128) RR -= 256;
  calib_homePos();
  Bipedal_Robot.saveTrimsOnEEPROM();
  EEPROM.commit();
  Bipedal_Robot.home();
  Serial.println("ROBOT CALIBRATION PROGRAM");
  Serial.println("PRESS q or a for adjusting Left Leg");
  Serial.println("PRESS w or s for adjusting Left Foot");
  Serial.println("PRESS e or d for adjusting Right Leg");
  Serial.println("PRESS r or f for adjusting Right Foot");
  Serial.println();
  Serial.println("PRESS t to test Robot walking");
  Serial.println("PRESS g to return servos to home position");
}

void loop() {
  if ((Serial.available()) > (0)) {
    charRead = Serial.read();
    Serial.println("YL: " + String(YL) + " YR: " + String(YR) + " RL: " + String(RL) + " RR: " + String(RR));
  }
  switch (charRead) {
    case 'q':
      YL++;
      Bipedal_Robot.setTrims(YL, YR, RL, RR);
      calib_homePos();
      Bipedal_Robot.saveTrimsOnEEPROM();
      break;
    case 'a':
      YL--;
      Bipedal_Robot.setTrims(YL, YR, RL, RR);
      calib_homePos();
      Bipedal_Robot.saveTrimsOnEEPROM();
      break;
    case 'w':
      RL++;
      Bipedal_Robot.setTrims(YL, YR, RL, RR);
      calib_homePos();
      Bipedal_Robot.saveTrimsOnEEPROM();
      break;
    case 's':
      RL--;
      Bipedal_Robot.setTrims(YL, YR, RL, RR);
      calib_homePos();
      Bipedal_Robot.saveTrimsOnEEPROM();
      break;
    case 'e':
      YR++;
      Bipedal_Robot.setTrims(YL, YR, RL, RR);
      calib_homePos();
      Bipedal_Robot.saveTrimsOnEEPROM();
      break;
    case 'd':
      YR--;
      Bipedal_Robot.setTrims(YL, YR, RL, RR);
      calib_homePos();
      Bipedal_Robot.saveTrimsOnEEPROM();
      break;
    case 'r':
      RR++;
      Bipedal_Robot.setTrims(YL, YR, RL, RR);
      calib_homePos();
      Bipedal_Robot.saveTrimsOnEEPROM();
      break;
    case 'f':
      RR--;
      Bipedal_Robot.setTrims(YL, YR, RL, RR);
      calib_homePos();
      Bipedal_Robot.saveTrimsOnEEPROM();
      break;
    case 't':
      Bipedal_Robot.walk(1, 2000, 1);
      Bipedal_Robot.home();
      break;
    case 'g':
      Bipedal_Robot.home();
      EEPROM.commit();
      break;
    case '2':
      Bipedal_Robot.walk(5, 2000, -1);
      break;
    case '3':
      Bipedal_Robot.turn(5, 2000, 1);
      break;
    case '4':
      Bipedal_Robot.turn(5, 2000, -1);
      break;
    case 'o':
      Bipedal_Robot.setTrims(YL, YR, RL, RR);
      calib_homePos();
      Bipedal_Robot.saveTrimsOnEEPROM();
      break;
    default:
      break;
  }
}

void calib_homePos() {
  int servoPos[4];
  servoPos[0] = 90;
  servoPos[1] = 90;
  servoPos[2] = 90;
  servoPos[3] = 90;
  Bipedal_Robot._moveServos(500, servoPos);
  // Bipedal_Robot.detachServos();
}
