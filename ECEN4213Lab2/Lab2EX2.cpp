// Lab 2 Exercise 2
//Mariia Shevchenko & Cin Sang 
//Use g++ -std=c++11 -o Lab2EX2 Lab2EX2.cpp -lwiringPi

#include <iostream>
#include <iomanip>
#include <unistd.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <chrono>
#include <cmath>

using namespace std::chrono;
using namespace std;

// functions
void sigroutine(int);
int adcVal();
void PID(float, float, float);
float read_potentionmeter();
float read_sonar();


// variables
float distance_previous_error, distance_error;
float obj_value = 0.0f; // potentionmeter reading
float measured_value = 0.0f; // sonar reading
int adc;
float PID_p,  PID_d, PID_total, PID_i = 0;
int time_inter_ms = 23; // time interval, you can use different time interval
   
int motor_pin = 26;
int sonar_pin = 1;
float kp = 2; 
float ki = 0.0005; 
float kd = 2;

int main(){
	wiringPiSetup();
    adc = wiringPiI2CSetup(0x48);

    /*Set the pinMode (fan pin)*/
    pinMode(motor_pin,PWM_OUTPUT);
    adc = wiringPiI2CSetup(0x48);

    
    // This part is to set a system timer, the function "sigroutine" will be triggered  
    // every time_inter_ms milliseconds. 
    struct itimerval value, ovalue;
    signal(SIGALRM, sigroutine);
    value.it_value.tv_sec = 0;
    value.it_value.tv_usec = time_inter_ms*1000;
    value.it_interval.tv_sec = 0;
    value.it_interval.tv_usec = time_inter_ms*1000;
    setitimer(ITIMER_REAL, &value, &ovalue);    

	while(true){
        cout<<"obj_value: "<<obj_value<<" measured_value: "<<measured_value<<endl;
        cout<<"PID_p: "<<PID_p<<endl;
        cout<<"PID_i: "<<PID_i<<endl;
        cout<<"PID_d: "<<PID_d<<endl;
        cout<<"PID_total: "<<PID_total<<endl;
        delay(20);
	}
}


void sigroutine(int signo){
    PID(kp, ki, kd);
    return;
}


/* based on the obj distance and measured distance, implement a PID control algorithm to 
the speed of the fan so that the Ping-Pang ball can stay in the obj position*/
void PID(float kp, float ki, float kd){
    /*read the objective position/distance of the ball*/
    obj_value = read_potentionmeter();
    /*read the measured position/distance of the ball*/
    measured_value = read_sonar();
    /*calculate the distance error between the obj and measured distance */
    distance_error = measured_value - obj_value;
    /*calculate the proportional, integral and derivative output */
    //adjust
    if (obj_value > 50 && obj_value <= 60)
    {
        kp = 3;
        ki = 0.001;
        kd = 50;
    }
    if (obj_value > 60 && obj_value <= 80){
        kp = 5;
        ki = 0.0011;
        kd = 70;
    }
    PID_p = kp * distance_error;
    PID_i = PID_i + (ki * distance_error*time_inter_ms);
    PID_d = kd*((distance_error - distance_previous_error)/time_inter_ms);
    PID_total = PID_p + PID_d + PID_i; 

    /*assign distance_error to distance_previous_error*/
    distance_previous_error = distance_error; //Adjust
    if (PID_total < 0)
       PID_total = 0;
    if (PID_total > 1024)
        PID_total = 1024;
    /*use PID_total to control your fan*/
    pwmWrite(motor_pin, int(PID_total));
}


/* use a sonar sensor to measure the position of the Ping-Pang ball. you may reuse
your code in EX1.*/
float read_sonar()
{
    pinMode(sonar_pin, OUTPUT);
    digitalWrite(sonar_pin, LOW);
    usleep(2);
    
    digitalWrite(sonar_pin, HIGH);
    usleep(5);
    
    digitalWrite(sonar_pin, LOW);
    
    usleep(750);
    pinMode(sonar_pin, INPUT);
    usleep(750);
    
    high_resolution_clock::time_point start = high_resolution_clock::now(); 
    float pulse_width;
    while(digitalRead(sonar_pin)){
       high_resolution_clock::time_point end = high_resolution_clock::now(); 
       pulse_width =  chrono::duration_cast<chrono::microseconds>(end - start).count();
       if(pulse_width>=18500)
            break;      
    }
    double distance = (pulse_width * 0.034 / 2);
    usleep(6000);

    return distance;
    
}

/* use a potentiometer to set an objective position (10 - 90 cm) of the Ping-Pang ball, varying the potentiometer
can change the objective distance. you may reuse your code in Lab 1.*/
float read_potentionmeter()
{
    float data;
    float sum = 0;
    int buff = 1;
    double voltage;
    for (int i = 0;i < buff; i++)
    {
       data  = adcVal();
       sum += data;
    }
    data = sum/buff;
    if (data > 1800)
       data = 0;
    voltage = (data/2047.0) * 6.144; // fs=6.144
    float distance = data * 100/1742;
    if (distance < 5)
       distance = 5;
    if (distance > 90)
       distance = 90;

   return distance;
}



int adcVal(){
	uint16_t low, high, value;
	wiringPiI2CWriteReg16(adc, 0X01,  0Xc5c1);
	usleep(1000);
    uint16_t data = wiringPiI2CReadReg16(adc,  0X00);
    low = (data & 0xFF00) >> 8;
    high = (data & 0x00FF) << 8;
    value = (high | low)>>4;
	return value;
}
