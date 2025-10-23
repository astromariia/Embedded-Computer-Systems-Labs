//Use g++ -std=c++11 -o Lab4EX1 Lab4EX1.cpp -lwiringPi

#include <string>
#include <iostream>
#include <wiringSerial.h>
#include <wiringPi.h>
#include <unistd.h>
#include <cstdlib>
#include <iomanip>
using namespace std;

int kobuki;

int main(){
	wiringPiSetup();
	kobuki = serialOpen("/dev/kobuki", 115200);
	unsigned int bumper;
	unsigned int drop;
	unsigned int cliff;
	unsigned int button;
	unsigned int read;

	while(serialDataAvail(kobuki) != -1){

		while(true){
		//If the bytes are a 1 followed by 15, then we are
		//parsing the basic sensor data packet
			read = serialGetchar(kobuki);
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

	return(0);
}
