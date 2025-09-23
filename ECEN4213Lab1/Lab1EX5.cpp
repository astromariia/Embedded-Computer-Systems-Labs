// Lab 1 Exercise 5
//Mariia Shevchenko & Cin Sang

// g++ -std=c++11 -o Lab1EX5 Lab1EX5.cpp -lwiringPi

#include <wiringPi.h>
#include <softPwm.h>
#include <stdio.h>
#include <wiringPiI2C.h>
#include <string.h>
#include <iostream>
#include <softPwm.h>
#include <math.h>
#include <stdlib.h>
#include <ctime>
#include <signal.h>
#include <iomanip>
#include <unistd.h>


#define SERVO_MIN_MS 5
#define SERVO_MAX_MS 25
#define SERVO_MIN_ANGLE 0
#define SERVO_MAX_ANGLE 180
using namespace std;

/* signal pin of the servo*/
#define servoPin 1

#define buttonPin 4
int adcVal(); 

int  direction = 1;

//Specific a certain rotation angle (0-180) for the servo
void servoWrite(int pin, int angle){ 
    long time = 0;
    //time = SERVO_MIN_MS + (angle - SERVO_MIN_ANGLE) * (SERVO_MAX_MS - SERVO_MIN_MS)/ (SERVO_MAX_ANGLE - SERVO_MIN_ANGLE);
    time = 10*(0.5 + (angle/90.0));
    softPwmWrite(pin,time);   
}


/* Sefind your callback function to handout the pressing button interrupts. */
void press_button()
{
    if(digitalRead(buttonPin)){
        direction = 1;
    }
    else {
        direction = 0;
    }
}




int main(void)
{

    wiringPiSetup();    
    softPwmCreate(servoPin,  0, 200);

    /* Use wiringPiISR() to setup your interrupts. Refer to document WiringPi_ Interrupts.pdf. */
    wiringPiISR(buttonPin, INT_EDGE_FALLING, &press_button);



    while(1){

        /* read ADS1015 value */
        int adcNumber = adcVal();
        
        /* convert the obtained ADS1015 value to angle 0 - 180*/
        int angle = (adcNumber * 180.0)/ 1749.0;
        cout << angle << endl;
        /* use the angle to control the servo motor*/
        if(direction==0){
            servoWrite(servoPin, angle);
        }
        else{
            servoWrite(servoPin, 180.0 - angle);
        }

        usleep(100000);

    }
    return 0;
}

//This function is used to read data from ADS1015
int adcVal(){

	uint16_t low, high, value;
	// Refer to the supplemental documents to find the parameters. In this lab, the ADS1015
	// needs to be set in single conversion, single-end mode, and FSR (full-scale range)is 6.144
    // you can choose any input pin (A0, A1, A2, A3) you like.
	int adc = wiringPiI2CSetup(0x48);/*Address of ADS1015(HEX)*/
	wiringPiI2CWriteReg16(adc, 0x01, 0xC5C1); //0b100000011010000 /*Configuration Register address pointer(HEX)*//*Configuration Register*/
	usleep(1000);

    // Read the configuration register to check the conversion status
    uint16_t config = wiringPiI2CReadReg16(adc, 0x01); /*Configuration Register address pointer(HEX)*/
    if (config & 0x8000) { // Check if the OS bit (bit 15) is set to 1
        // read the conversion result
        uint16_t data = wiringPiI2CReadReg16(adc, 0x00); /*Conversion Register address pointer(HEX)*/
        low = (data & 0xFF00) >> 8;
        high = (data & 0x00FF) << 8;
        value = (high | low)>>4;
    }
	return value;
}
