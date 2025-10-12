//Use g++ joystick.cc -std=c++11 -o Lab3EX2 Lab3EX2.cpp -lwiringPi

#include <iostream>
#include <wiringPi.h>
#include <wiringSerial.h>
#include "joystick.hh"
#include <unistd.h>
#include <cstdlib>
using namespace std;

void movement(int, int);
int kobuki;

int main(){

	wiringPiSetup();
	kobuki = serialOpen("/dev/kobuki", 115200);
	Joystick joystick("/dev/input/js0");
	JoystickEvent event;
	unsigned int button;

	//The joystick creates events when a button or axis changes value.
	//Sample event from the joystick: joystick.sample(&event)

	//You can interpret these by sampling the events.
	//Each event has three parameters.
	//A type, axis or button,
	//judge if the event is button: event.isButton()
	//judge if the event is axis: event.isAxis()
	//A number corresponding to the axis or button pressed: event.number
	//And a value, Buttons: 0-unpressed, 1-pressed, Axis: -32767 to 0 to 32767: event.value

	while(true){
		/*Create a series of commands to interpret the
		joystick input and use that input to move the Kobuki*/

		//Use the following Key Map:
		//Up     - move the Kobuki forward: eventnum = 7, value -32767
		//Down   - move the Kobuki backward: eventnum = 7, value 32767
		//Left   - rotate the Kobuki 90 degrees counterclockwise: eventnum 6, value -32767
		//Right  - rotate the Kobuki 90 degrees clockwise: event num 6, value 32767
		//Start  - immediately stop the Kobuki's movement: event num 7, value 1
		//Select - exit the script and close the Kobuki's connection cleanly: event num 8, value 1
		if (joystick.sample(&event))
		{
			if (event.isButton())
			{
				printf("isButton: %u | Value: %d\n", event.number, event.value);
				/*Interpret the joystick input and use that input to move the Kobuki*/
				if(event.number== 7 && event.value==1){
					movement(0,0);
				}
				if(event.number== 8 && event.value == 1){
					serialClose(kobuki);
				}

			}
			if (event.isAxis())
			{
				printf("isAxis: %u | Value: %d\n", event.number, event.value);
				/*Interpret the joystick input and use that input to move the Kobuki*/
				if (event.number == 6){
					if (event.value < 0){
						movement(80, 1);
					}
					else if(event.value > 0){
						movement(80,-1);					
					}
				}
				if (event.number == 7){
					if(event.value < 0){
						movement(150,0);

					}
					else if(event.value > 0){
						movement(-150,0);

					}
				}


				
			}
		}

	}

	return 0;
}

void movement(int sp, int r){
	//Create the byte stream packet with the following format:
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
