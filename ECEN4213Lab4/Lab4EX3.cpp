//Use g++ -std=c++11 -o Lab4EX3 Lab4EX3.cpp -lwiringPi

#include <string>
#include <iostream>
#include <wiringSerial.h>
#include <wiringPi.h>
#include <unistd.h>
#include <cstdlib>
#include <iomanip>
using namespace std;

int kobuki;

unsigned int bumper;
unsigned int drop;
unsigned int cliff;
unsigned int button;
//unsigned int read; 

void movement(int, int);
void readData();

int main(){
	//Create connection to the Kobuki
	wiringPiSetup();
	kobuki = serialOpen("/dev/kobuki", 115200);

	while(serialDataAvail(kobuki) != -1){
		/*Read the initial data. If there are no flags,
		the default condition is forward.*/
		/*Move slowly to give the sensors enough time to read data,
		the recommended speed is 100mm/s*/
		readData();
		movement(60,0);
		/*Create different states as to satisfy the conditions above.
		Remember, a single press of a bumper may last longer
		than one data cycle.*/
		if(bumper == 1) { // bumper right - turn left
            movement(85, 1);
            sleep(2);
			movement(100, 0);
            sleep(2);
        }
		if(bumper == 2) { // bumper forward - reverse
			movement(-100, 0);
			sleep(2);
		}
		if(bumper == 3){ //bumper right forward
			movement(-100, 0);
            sleep(2);
            movement(85, 1);
            sleep(2);
		}
		if (bumper == 4){ // bumper left
			movement(85, -1);
            sleep(2);
			movement(100, 0);
            sleep(2);
		}
		if (bumper == 6){ // bumper left forward
			movement(-100, 0);
            sleep(2);
            movement(85, -1);
            sleep(2);
		}
		if(cliff > 0){ //cliff = 2 Cliff Forward
			movement(-100,0);
			sleep(2);
		}

		

		/*Cleanly close out of all connections using Button 1.*/
        if(button == 2) {
            serialClose(kobuki);
        }
		/*Use serialFlush(kobuki) to discard all data received, or waiting to be send down the given device.*/
        serialFlush(kobuki);
	}
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

void readData(){
	while(true){
		//If the bytes are a 1 followed by 15, then we are
		//parsing the basic sensor data packet
			char read = serialGetchar(kobuki);
			if(read == 1){
				if(serialGetchar(kobuki) == 15) break;
			}
		}

		//Read past the timestamp
		serialGetchar(kobuki);
		serialGetchar(kobuki);

		/*Read the bytes containing the bumper, wheel drop,
			and cliff sensors. You can convert them into a usable data type.*/
		bumper = serialGetchar(kobuki);
		drop = serialGetchar(kobuki);
		cliff = serialGetchar(kobuki);
		/*Print the data to the screen.*/
		if (bumper == 1){
			cout << "Bumper Right: "<< bumper << endl;
		}
		else if (bumper == 2){
			cout << "Bumper Forward: "<< bumper << endl;
		}
		else if (bumper == 4){
			cout << "Bumper Left: "<< bumper << endl;
		}
		else if (bumper == 3){
			cout << "Bumper Right Forward: "<< bumper << endl;
		}
		else if (bumper == 6){
			cout << "Bumper Left Forward: "<< bumper << endl;
		}
		else if (bumper == 5){
			cout << "Bumper Left Right: "<< bumper << endl;
		}
		else if (bumper == 7){
			cout << "Bumper Left Right Forward: "<< bumper << endl;
		}
		/*Read through 6 bytes between the cliff sensors and
		the button sensors.*/
		else if (cliff == 1){
			cout << "Cliff Right: "<< cliff << endl;
		}
		else if (cliff == 2){
			cout << "Cliff Forward: "<< cliff << endl;
		}
		else if (cliff == 4){
			cout << "Cliff Left: "<< cliff << endl;
		}
		else if (cliff == 3){
			cout << "Cliff Right Forward: "<< cliff << endl;
		}
		else if (cliff == 6){
			cout << "Cliff Left Forward: "<< cliff << endl;
		}
		else if (cliff == 5){
			cout << "Cliff Left Right: "<< cliff << endl;
		}
		else if (cliff == 7){
			cout << "Cliff Left Right Forward: "<< cliff << endl;
		}
		else if (drop == 3){
			cout << "Drop Both Wheels: "<< drop << endl;
		}
		else if (drop == 1){
			cout << "Drop Right Wheel: "<< drop << endl;
		}
		else if (drop == 2){
			cout << "Drop Left Wheel: "<< drop << endl;
		}
		/*Read the byte containing the button data.*/
		serialGetchar(kobuki);
		serialGetchar(kobuki);
		serialGetchar(kobuki);
		serialGetchar(kobuki);
		serialGetchar(kobuki);
		serialGetchar(kobuki);

		button = serialGetchar(kobuki);
		/*Close the script and the connection to the Kobuki when
		Button 1 on the Kobuki is pressed. Use serialClose(kobuki);*/
		if (button == 2){
			serialClose(kobuki);
		}
		

		//Pause the script so the data read receive rate is the same as the Kobuki send rate.
		usleep(20000);
			
		
	}
