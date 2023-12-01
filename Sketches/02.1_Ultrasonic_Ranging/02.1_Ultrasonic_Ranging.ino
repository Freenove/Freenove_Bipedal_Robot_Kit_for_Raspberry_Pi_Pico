/**********************************************************************
  Product     : Freenove Robot for Raspberry Pi Pico (W)
  Description : Ultrasonic ranging.
  Auther      : www.freenove.com
  Modification: 2023/11/07
**********************************************************************/
#include <Arduino.h>
#include "Freenove_Robot_For_Pico_W.h"

void setup() {
  Serial.begin(115200);//Open the serial port and set the baud rate to 115200
  Ultrasonic_Setup();  //Ultrasonic module initialization
  delay(500);          //Wait for the servo to arrive at the specified location
}

void loop() {
  Serial.print("Distance: " + String(Get_Sonar()) + "\n");//Print ultrasonic distance
  delay(500);
}
