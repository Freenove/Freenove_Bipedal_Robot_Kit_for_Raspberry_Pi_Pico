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

String inputString = "";      // a String to hold incoming data
bool stringComplete = false;  // whether the string is complete

#define COMMANDS_COUNT_MAX 8
char charRead;
String inputStringBLE = "";

int move = 0;

#define ACTION_calibratio 'G'  //Ant movement commands
#define ACTION_flag 'F'        //Expression control commands
#define ACTION_MOVE 'A'

#define INTERVAL_CHAR '#'       //The directive resolves the separator character
String inputCommandArray[COMMANDS_COUNT_MAX];
int paramters[COMMANDS_COUNT_MAX];

//Parses the Bluetooth data received by the serial port
void Deal_Serial_Data(void) {
  String inputStringTemp = inputStringBLE;
  int string_length = inputStringTemp.length();
  for (int i = 0; i < COMMANDS_COUNT_MAX; i++) {
    int index = inputStringTemp.indexOf(INTERVAL_CHAR);
    if (index < 0) {
      if (string_length > 0) {
        inputCommandArray[i] = inputStringTemp;  //Get command
        paramters[i] = inputStringTemp.toInt();  //Get parameters
      }
      break;
    }
    inputCommandArray[i] = inputStringTemp.substring(0, index);
    inputStringTemp = inputStringTemp.substring(index + 1);
    paramters[i] = inputCommandArray[i].toInt();
  }
  stringComplete = false;
  inputStringBLE = "";
}


void setup() {
  EEPROM.begin(512);
  Serial.begin(9600);
  // for (int i = 0; i < 4; i++) {
  //   EEPROM.write(i, calibratePosition[i]);
  // }
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
}

void loop() {
  while (Serial.available()) {
    inputStringBLE = Serial.readStringUntil('\n');
    stringComplete = true;
  }
  if (stringComplete) {  //Get the Bluetooth command
    Deal_Serial_Data();
    char commandChar = inputCommandArray[0].charAt(0);
    if (commandChar == ACTION_calibratio) {  //Control expression module display
      // Serial.println("paramters[1]: " + String(paramters[1]) + " paramters[2]: " + String(paramters[2]) + " paramters[3]: " + String(paramters[3]) + " paramters[4]: " + String(paramters[4]) + " paramters[5]: " + String(paramters[5]));
      YL = constrain(paramters[1] - 90, -40, 50);
      YR = constrain(paramters[2] - 90, -40, 50);
      RL = constrain(paramters[3] - 90, -40, 50);
      RR = constrain(paramters[4] - 90, -40, 50);
      Bipedal_Robot.setTrims(YL, YR, RL, RR);
      calib_homePos();
      if (paramters[5] = 1) {
        Bipedal_Robot.setTrims(YL, YR, RL, RR);
        calib_homePos();
        Bipedal_Robot.saveTrimsOnEEPROM();
      }
    }
    if (commandChar == ACTION_flag) {  //Control expression module display
      if (paramters[1] = 1) {
        YL = EEPROM.read(0);
        if (YL > 128) YL -= 256;
        YR = EEPROM.read(1);
        if (YR > 128) YR -= 256;
        RL = EEPROM.read(2);
        if (RL > 128) RL -= 256;
        RR = EEPROM.read(3);
        if (RR > 128) RR -= 256;
        YL = constrain(YL + 90, 50, 140);
        YR = constrain(YR + 90, 50, 140);
        RL = constrain(RL + 90, 50, 140);
        RR = constrain(RR + 90, 50, 140);
        Serial.print("F#"+String(YL)+"#"+String(YR)+"#"+String(RL)+"#"+String(RR));
      }
    }
if (commandChar == ACTION_MOVE) {  //Control expression module display
    move = paramters[1];
    }
  }
  if (move == 1) {
      Bipedal_Robot.walk(1, 1500, 1);
    }
    if (move == 2) {
      Bipedal_Robot.walk(1, 1500, -1);
    }
    if (move == 3) {
      Bipedal_Robot.turn(1, 1500, 1);
    }
    if (move == 4) {
      Bipedal_Robot.turn(1, 1500, -1);
    }
    if (move == 0) {
      Bipedal_Robot.home();
    }
}

void calib_homePos() {
  int servoPos[4];
  servoPos[0] = 90;
  servoPos[1] = 90;
  servoPos[2] = 90;
  servoPos[3] = 90;
  Bipedal_Robot._moveServos(500, servoPos);
}