/**********************************************************************
  Product     : Freenove Robot for Raspberry Pi Pico (W)
  Description : Servos test.
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
#include <EEPROM.h>
#include "Bipedal_Robot.h"
Bipedal_Robot Bipedal_Robot;

#define LeftLeg 10
#define RightLeg 12
#define LeftFoot 11
#define RightFoot 13

// int calibratePosition[4] = { -17, -10, -15, 12 };

void setup() {
  Serial.begin(115200);
  EEPROM.begin(512);
  Bipedal_Robot.init(LeftLeg, RightLeg, LeftFoot, RightFoot, true);  //Set the servo pins 
  // Bipedal_Robot.setTrims(calibratePosition[0], calibratePosition[1], calibratePosition[2], calibratePosition[3]);
  calib_homePos();
  Bipedal_Robot.saveTrimsOnEEPROM();
  EEPROM.commit();
  Bipedal_Robot.home();
  delay(50);
}
void loop() {
  Serial.println("loop.....");
  Bipedal_Robot.walk(2,2000,1);
  Bipedal_Robot.home();
  delay(1000);
  Bipedal_Robot.walk(2,2000,-1);
  Bipedal_Robot.home();
  delay(1000);
  Bipedal_Robot.turn(2,2000,1);
  Bipedal_Robot.home();
  delay(1000);
  Bipedal_Robot.turn(2,2000,-1);
  Bipedal_Robot.home();
  delay(1000);
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