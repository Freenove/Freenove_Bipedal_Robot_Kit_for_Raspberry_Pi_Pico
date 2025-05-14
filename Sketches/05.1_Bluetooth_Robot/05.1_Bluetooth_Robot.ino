/**********************************************************************
  Filename    : Receive_Bluetooth_Data.ino
  Product     : Freenove Robot for Raspberry Pi Pico (W)
  Description : Receive data from bluetooth and print it to monitor.
  Auther      : www.freenove.com
  Modification: 2023/11/07
**********************************************************************/
#include <Arduino.h>
#include <SoftwareSerial.h>  // Reference library

#include "SerialCommand.h"
SoftwareSerial BTserial = SoftwareSerial(1, 0);  // RX pin to 0 and TX pin to 1 on the board
SerialCommand SCmd(BTserial);

String inputString = "";    // a String to hold incoming data
bool stringComplete = false;// whether the string is complete
int baudRate[] = {2400, 9600, 19200, 115200};  
int baudRateCount = sizeof(baudRate) / sizeof(baudRate[0]);  

void receiveStop() {
  delay(10);
  Serial.flush();
}

void setup() {
  Serial.begin(9600);
  delay(2000);
  // Loop through each baud rate and send commands 
  for(int i = 0; i < baudRateCount; i++){
    BTserial.begin(baudRate[i]);
    delay(200);
    BTserial.println("AT+NAME=BT05");//Set the radio called Robot
    delay(200);
    BTserial.println("AT+ROLE=0");
    delay(200);
    BTserial.println("AT+UART=2");//1=2400 2=9600 3=19200 4=115200
    delay(200);

    //SoftwareSerial does not support dynamic adjustment of baud rate, so close the serial
    BTserial.end();
  }
  
  BTserial.begin(9600);
  delay(2000);

  SCmd.addDefaultHandler(receiveStop);
  inputString.reserve(200);
  Serial.println("Set the name of the Bluetooth module to BT05.");
  Serial.println("Set the Bluetooth baud rate to 9600.");
  // Serial.println("Set Bluetooth to slave mode.");
}

void loop() {
  while (BTserial.available()) {
    // get the new byte:
    char inChar = (char)BTserial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
    if (stringComplete) {
      Serial.println(inputString);
      // clear the string:
      inputString = "";
      stringComplete = false;
    }
  }
}
