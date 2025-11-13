//use g++ -std=c++11 -o Lab4EX2 Lab4EX2.cpp -lwiringPi


#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <wiringPi.h>
#include <wiringSerial.h>
#include <ctime>
#include <ratio>
#include <chrono>
using namespace std;
using namespace std::chrono;

int kobuki;
int sonar_pin = 4;
float read_sonar();
void movement(int, int);

high_resolution_clock::time_point  start_time;
high_resolution_clock::time_point  end_time;
int out;

int main(){
	wiringPiSetup();
	kobuki = serialOpen("/dev/kobuki", 115200);
	//movement(0,0);
	int counter = 0;
	int value = 0;
	/*Move from a random point within the area designated "X" to the
	point B as shown on the diagram. Use a sonar sensor to navigate through the channel.
	You can reuse your code from Lab 2 and 3*/
	while(true){
		value = read_sonar();
		cout << "Sonar: " << value << endl;
		//sleep(1);
		//movement(150,0);
		//sleep(4);
		cout << value << endl;
		if (value < 15){
			cout << "Obstacle detected at distance: "<< value << endl; 
			break;
			//sleep(1);
			//++counter;
		}
		else{
			cout << "Moving Forward: "<< value << endl; 
			movement(150,0);
		}
	}

	start_time = chrono::high_resolution_clock::now();
    while(true){
		cout << "First move";
        movement(90, -1);
		sleep(2);
        end_time = chrono::high_resolution_clock::now();
        out = chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();
        if (out>3) break;
    }
    while(true){
		value = read_sonar();
		cout << "Sonar: " << value << endl;
		cout << value << endl;
		if (value < 20){
			cout << "Obstacle detected at distance: "<< value << endl; 
			break;
			sleep(1);
			++counter;
		}
		else{
			cout << "Moving Forward: "<< value << endl; 
			movement(150,0);
		}
	}
    start_time = chrono::high_resolution_clock::now();
    while(true){
        cout << "Second turn";
        movement(90, 1);
        sleep(5);
        end_time = chrono::high_resolution_clock::now();
        out = chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();
        if (out>6)
            break;
    }
    //start_time = chrono::high_resolution_clock::now();
    while(true){
		value = read_sonar();
		cout << "Sonar: " << value << endl;
		cout << value << endl;
		if (value < 20){
			cout << "Obstacle detected at distance: "<< value << endl; 
			break;
		}
		else{
			cout << "Moving Forward: "<< value << endl; 
			movement(150,0);
		}
	}
    while(true){
    	movement(0,0);
    	sleep(4);
    	serialClose(kobuki);
    }
	/*Note: the Kobuki must completely pass point B as shown to receive full credit*/


}


float read_sonar()
{
    //low
    pinMode(sonar_pin, OUTPUT);
    digitalWrite(sonar_pin, LOW);
    usleep(2);
    //high
    digitalWrite(sonar_pin, HIGH);
    usleep(5);
    //low
    digitalWrite(sonar_pin, LOW);

    pinMode(sonar_pin, INPUT);
    usleep(750);
    high_resolution_clock::time_point start = high_resolution_clock::now();
    float dura;
    while (digitalRead(sonar_pin))
    {
        high_resolution_clock::time_point end = high_resolution_clock::now();
        dura =  chrono::duration_cast<chrono::microseconds>(end - start).count();
        if(dura>=18500)
        // if(dura>=6167)
            break;
    }
    float distance = (17 * dura) / 1000;
    usleep(2000);

    return distance;
}




void movement(int sp, int r){
	unsigned char b_0 = 0xAA; /*Byte 0: Kobuki Header 0*/
	unsigned char b_1 = 0x55; /*Byte 1: Kobuki Header 1*/
	unsigned char b_2 = 0x6; /*Byte 2: Length of Payload*/
	unsigned char b_3 = 0x01; /*Byte 3: Sub-Payload Header (Base control)*/
	unsigned char b_4 = 0x4; /*Byte 4: Length of Sub-Payload*/

	unsigned char b_5 = sp & 0xff;	//Byte 5: Payload Data: Speed(mm/s)
	unsigned char b_6 = (sp >> 8) & 0xff; //Byte 6: Payload Data: Speed(mm/s)
	unsigned char b_7 = r & 0xff;	//Byte 7: Payload Data: Radius(mm)
	unsigned char b_8 = (r >> 8) & 0xff;	//Byte 8: Payload Data: Radius(mm)
	unsigned char checksum = 0;		//Byte 9: Checksum
	
	//Checksum all of the data
	char packet[] = {b_0,b_1,b_2,b_3,b_4,b_5,b_6,b_7,b_8};
	for (unsigned int i = 2; i < 9; i++)
		checksum ^= packet[i];

	/*Send the data (Byte 0 - Byte 8 and checksum) to Kobuki using serialPutchar (kobuki, );*/
	serialPutchar(kobuki, b_0);
	serialPutchar(kobuki, b_1);
	serialPutchar(kobuki, b_2);
	serialPutchar(kobuki, b_3);
	serialPutchar(kobuki, b_4);
	serialPutchar(kobuki, b_5);
	serialPutchar(kobuki, b_6);
	serialPutchar(kobuki, b_7);
	serialPutchar(kobuki, b_8);

	serialPutchar(kobuki, checksum);

	/*Pause the script so the data send rate is the
	same as the Kobuki data receive rate*/
	usleep(20000);
	
}