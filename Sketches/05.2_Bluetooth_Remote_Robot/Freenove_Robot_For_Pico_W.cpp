#include <Arduino.h>
#include <Wire.h>
#include "Freenove_Robot_For_Pico_W.h"

#include "Bipedal_Robot.h"
extern Bipedal_Robot Bipedal_Robot;  //This is Bipedal_Robot!

//////////////////////Buzzer drive area///////////////////////////////////
void Buzzer_Setup(void) {
  pinMode(PIN_BUZZER, OUTPUT);
}

void Buzzer_Variable(int frequency, int time, int times) {
  for (int i = 0; i < times; i++) {
    tone(2, frequency);
    delay(time);
    tone(2, 0);
    delay(time);
  }
}

void Buzzer_Alarm(bool enable) {
  if (enable == 1) {
    freq(PIN_BUZZER, 2000, 30);
    delay(500);
    freq(PIN_BUZZER, 0, 30);
    delay(500);
  } else
    freq(PIN_BUZZER, 0, 30);
}

//Buzzer alarm function
void Buzzer_Alert(int beat, int rebeat) {
  beat = constrain(beat, 1, 9);
  rebeat = constrain(rebeat, 1, 255);
  for (int j = 0; j < rebeat; j++) {
    for (int i = 0; i < beat; i++) {
      freq(PIN_BUZZER, BUZZER_FREQUENCY, 10);
    }
    delay(500);
  }
  freq(PIN_BUZZER, 0, 10);
}

void freq(int PIN, int freqs, int times) {
  if (freqs == 0) {
    digitalWrite(PIN, LOW);
  } else {
    for (int i = 0; i < times * freqs / 500; i++) {
      digitalWrite(PIN, HIGH);
      delayMicroseconds(500000 / freqs);
      digitalWrite(PIN, LOW);
      delayMicroseconds(500000 / freqs);
    }
  }
}

////////////////////Battery drive area/////////////////////////////////////
float batteryVoltage = 0;      //Battery voltage variable
float batteryCoefficient = 4;  //Set the proportional coefficient
int oa_VoltageCompensationToSpeed;

//Gets the battery ADC value
int Get_Battery_Voltage_ADC(void) {
  pinMode(PIN_BATTERY, INPUT);
  int batteryADC = 0;
  for (int i = 0; i < 5; i++)
    batteryADC += analogRead(PIN_BATTERY);
  return batteryADC / 5;
}

//Get the battery voltage value
float Get_Battery_Voltage(void) {
  int batteryADC = Get_Battery_Voltage_ADC();
  batteryVoltage = (batteryADC / 1023.0 * 3.3) * batteryCoefficient;
  return batteryVoltage;
}

void Set_Battery_Coefficient(float coefficient) {
  batteryCoefficient = coefficient;
}

/////////////////////Ultrasonic drive area//////////////////////////////
//Ultrasonic initialization
void Ultrasonic_Setup(void) {
  pinMode(PIN_SONIC_TRIG, OUTPUT);  // set trigPin to output mode
  pinMode(PIN_SONIC_ECHO, INPUT);   // set echoPin to input mode
}

//Obtain ultrasonic distance data
float Get_Sonar(void) {
  unsigned long pingTime;
  float distance;
  digitalWrite(PIN_SONIC_TRIG, HIGH);  // make trigPin output high level lasting for 10μs to triger HC_SR04,
  delayMicroseconds(10);
  digitalWrite(PIN_SONIC_TRIG, LOW);
  pingTime = pulseIn(PIN_SONIC_ECHO, HIGH, SONIC_TIMEOUT);  // Wait HC-SR04 returning to the high level and measure out this waitting time
  if (pingTime != 0)
    distance = (float)pingTime * SOUND_VELOCITY / 2 / 10000;  // calculate the distance according to the time
  else
    distance = MAX_DISTANCE;
  // Serial.print("Distance: " + String(distance) + "\n");  //Print ultrasonic distance
  return distance;                                       // return the distance value
}

//Ultrasonic robot
void Ultrasonic_Avoid() {
    if (Get_Sonar() <= 15) {
      Serial.println("Ultrasonic Avoid mode... ");
      Serial.println(Get_Sonar());
      Bipedal_Robot.walk(2,1000,-1); // BACKWARD x2
      Bipedal_Robot.turn(3,1000,1); // LEFT x3
    }
    Bipedal_Robot.walk(1,1000,1); // FORWARD x1
    Serial.println("Ultrasonic mode... ");
}

int Check_Module_value = 0;
bool i2CAddrTest(uint8_t addr) {
  Wire.begin();
  Wire.beginTransmission(addr);
  if (Wire.endTransmission() == 0) {
    return true;
  }
  return false;
}

