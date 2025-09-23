// g++ -std=c++11 -o test_code test_code.cpp -lwiringPi

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

// Function prototype
int adcVal();

/* signal pin of the servo*/
#define servoPin 1
#define buttonPin 0  // Define the GPIO pin for the button
int direction = 1;

//Specific a certain rotation angle (0-180) for the servo
void servoWrite(int pin, int angle){
    long time = 0;
    time = 10*(.5+(angle/90.0));
    softPwmWrite(pin,time);  
}

volatile int current_angle = 0;  // Keep track of current angle
volatile bool button_pressed = false;  // Flag for button press

/* Callback function to handle button press interrupts */
void press_button()
{
    //button_pressed = true;  // Set flag to move to supplementary angle
    //delay(200);  // Debounce delay

    if (digitalRead(buttonPin))
    {
        while(digitalRead(buttonPin));
        if (direction)
            direction = 0;
        else
            direction = 1;
        cout<<"direction: "<<direction<<endl;
    }    
    usleep(500000);
}




int main(void)
{

    wiringPiSetup();    
    softPwmCreate(servoPin, 0, 200);

    // Set up the button pin as input with pull-up resistor
    pinMode(buttonPin, INPUT);
    pullUpDnControl(buttonPin, PUD_UP);  // Enable pull-up resistor

    // Set up interrupt for button press
    wiringPiISR(buttonPin, INT_EDGE_FALLING, &press_button);  // Trigger on falling edge




    while(1){
        /* read ADS1015 value */
        int adc_value = adcVal();
       
        /* convert the obtained ADS1015 value to angle 0 - 180 */
        current_angle = (adc_value * SERVO_MAX_ANGLE) / 2047;
       
        /* check if button was pressed */
        if (direction == 0) {
            /* move to supplementary angle (180° - current_angle) */
            current_angle = SERVO_MAX_ANGLE - current_angle;
            //button_pressed = false;  // Reset the flag
        } else {
           
        }
       
        /* use the angle to control the servo motor */
        servoWrite(servoPin, current_angle);
       
        usleep(100000);  // 100ms delay
    }
    return 0;
}

//This function is used to read data from ADS1015
int adcVal(){
    int adc;  // Device handle
    uint16_t low, high, value;
    // Refer to the supplemental documents to find the parameters. In this lab, the ADS1015
    // needs to be set in single conversion, single-end mode, and FSR (full-scale range)is 6.144
    // you can choose any input pin (A0, A1, A2, A3) you like.
    adc = wiringPiI2CSetup(0x48);
    wiringPiI2CWriteReg16(adc, 0x01, 0xC183);
    usleep(1000);

    // Read the configuration register to check the conversion status
    uint16_t config = wiringPiI2CReadReg16(adc, 0x01);  // Configuration register at 0x01
    if (config & 0x8000) { // Check if the OS bit (bit 15) is set to 1
        // read the conversion result
        uint16_t data = wiringPiI2CReadReg16(adc, 0x00);  // Conversion register at 0x00
        low = (data & 0xFF00) >> 8;
        high = (data & 0x00FF) << 8;
        value = (high | low)>>4;
    }
    return value;
}
