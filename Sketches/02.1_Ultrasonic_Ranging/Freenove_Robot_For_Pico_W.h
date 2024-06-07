#ifndef _FREENOVE_ROBOT_H
#define _FREENOVE_ROBOT_H

#include <Arduino.h>

//////////////////////Buzzer drive area///////////////////////////////////
//Buzzer pin definition             
#define PIN_BUZZER 2                    //Define the pins for the Pico W control buzzer
#define BUZZER_FREQUENCY 2000           //Define the resonant frequency of the buzzer 
void Buzzer_Setup(void);                //Buzzer initialization
void Buzzer_Alert(int beat, int rebeat);//Buzzer alarm function
void freq(int PIN, int freqs, int times); 

////////////////////Battery drive area/////////////////////////////////////
#define PIN_BATTERY        28        //Set the battery detection voltage pin
#define LOW_VOLTAGE_VALUE  525       //Set the minimum battery voltage
extern float batteryCoefficient;     //Set the proportional coefficient

int Get_Battery_Voltage_ADC(void);   //Gets the battery ADC value
float Get_Battery_Voltage(void);     //Get the battery voltage value
void Set_Battery_Coefficient(float coefficient);//Set the partial pressure coefficient

/////////////////////Ultrasonic drive area/////////////////////////////////
#define PIN_SONIC_TRIG    8             //define Trig pin
#define PIN_SONIC_ECHO    9             //define Echo pin
#define MAX_DISTANCE      300           //cm
#define SONIC_TIMEOUT (MAX_DISTANCE*60) // calculate timeout 
#define SOUND_VELOCITY    340           //soundVelocity: 340m/s
void Ultrasonic_Setup(void);//Ultrasonic initialization
float Get_Sonar(void);//Obtain ultrasonic distance data

#endif
