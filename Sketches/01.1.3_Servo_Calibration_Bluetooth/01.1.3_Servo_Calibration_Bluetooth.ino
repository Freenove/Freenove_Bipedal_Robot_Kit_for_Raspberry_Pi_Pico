/**********************************************************************
  Filename    : Receive_Bluetooth_Data.ino
  Product     : Freenove Robot for Raspberry Pi Pico (W)
  Description : Receive data from bluetooth and print it to monitor.
  Auther      : www.freenove.com
  Modification: 2023/11/07
*************************
*********************************************/
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
#include "SerialCommand.h"
#include "Bipedal_Robot.h"

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

#define COMMANDS_COUNT_MAX 5
char charRead;
String inputStringBLE = "";

#define ACTION_calibratio 'G'  //Ant movement commands
#define ACTION_flag 'F'        //Expression control commands
#define ACTION_MOVE 'A'

#define INTERVAL_CHAR '#'  //The directive resolves the separator character
String inputCommandArray[COMMANDS_COUNT_MAX];
int paramters[COMMANDS_COUNT_MAX];
int move = 0;
int movespeed = 1500;

SoftwareSerial BTserial = SoftwareSerial(1, 0);  // RX pin to 0 and TX pin to 1 on the board
SerialCommand SCmd(BTserial);
Bipedal_Robot Bipedal_Robot;

void receiveStop() {
  delay(10);
  Serial.flush();
}

//Parses the Bluetooth data received by the serial port
void Deal_Serial_Data(void) {
  String inputStringTemp = inputStringBLE;
  int string_length = inputStringTemp.length();
  Serial.println(inputStringTemp);
  for (int i = 0; i < COMMANDS_COUNT_MAX; i++) {
    int index = inputStringTemp.indexOf(INTERVAL_CHAR);
    if (index < 0) {
      if (19 > string_length > 0) {
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
  Serial.begin(115200);
  BTserial.begin(115200);
  delay(3000);
  BTserial.println("AT+UART?\r\n");
  delay(200);
  BTserial.println("AT+UART=115200\r\n");
  delay(200);
  BTserial.println("AT+NAME=Test\r\n");  //Set the radio called Robot
  delay(200);

  SCmd.addDefaultHandler(receiveStop);
  inputString.reserve(200);
  Serial.println("Set the name of the Bluetooth module to BT05.");
  Serial.println("Set the Bluetooth baud rate to 115200.");

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
  Serial.println("start...");
}

void readservo(int parameter1, int parameter2, int parameter3, int parameter4) {
  parameter1 = EEPROM.read(0);
  parameter2 = EEPROM.read(1);
  parameter3 = EEPROM.read(2);
  parameter4 = EEPROM.read(3);
}

void loop() {
  if (BTserial.available()) {
    inputStringBLE = BTserial.readStringUntil('\n');
    stringComplete = true;
  }
  if (stringComplete) {  //Get the Bluetooth command
    Deal_Serial_Data();
    char commandChar = inputCommandArray[0].charAt(0);
    if (commandChar == ACTION_calibratio) {  //Control expression module display
      YL = constrain(paramters[1] - 90, -40, 50);
      YR = constrain(paramters[2] - 90, -40, 50);
      RL = constrain(paramters[3] - 90, -40, 50);
      RR = constrain(paramters[4] - 90, -40, 50);
      Serial.println("F#" + String(YL) + "#" + String(YR) + "#" + String(RL) + "#" + String(RR));
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
        Serial.println("F#" + String(YL) + "#" + String(YR) + "#" + String(RL) + "#" + String(RR));
        BTserial.println("F#" + String(YL) + "#" + String(YR) + "#" + String(RL) + "#" + String(RR));
      }
    }
    if (commandChar == ACTION_MOVE) {  //Control expression module display
      move = paramters[1];
      movespeed = paramters[2];
      if (movespeed == 0) {
        movespeed = 1000;
      } else if (movespeed == 1) {
        movespeed = 1500;
      } else if (movespeed == 2) {
        movespeed = 2000;
      } else
        movespeed = 1500;
    }
  }
  if (move == 1) {
    Bipedal_Robot.walk(1, movespeed, 1);
  }
  if (move == 2) {
    Bipedal_Robot.walk(1, movespeed, -1);
  }
  if (move == 3) {
    Bipedal_Robot.turn(1, movespeed, 1);
  }
  if (move == 4) {
    Bipedal_Robot.turn(1, movespeed, -1);
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
  Bipedal_Robot._moveServos(100, servoPos);
}
