
#include "lib.h"


#define Pino1_TCS 9 	// S0
#define Pino2_TCS 10	// S1
#define Pino3_TCS 6		// S2
// Pino4_TCS GND		// GND
#define Pino5_TCS 4		// S3
// Pino6_TCS GND 		// OE
#define Pino7_TCS1 11	// 1st sensor output
#define Pino7_TCS2 8	// 2nd sensor output
#define Pino7_TCS3 7	// 3th sensor output
#define Pino7_TCS4 5	// 4th sensor output
// Pino8_TCS VCC

#define S0 10
#define S1 11
#define S2 12
#define S3 13

#define n_sensors 2

#define LED_CALIB 3
#define LED_RED   6
#define LED_GREEN 5
#define LED_BLUE  4


#define RED 0
#define GREEN 1
#define BLUE 2

//const int out[n_sensors] = {Pino7_TCS1, Pino7_TCS2};   
TCS s[4];

int incomingByte = 0;  


    
void setup()   
{  
	int i;
	pinMode(S0, OUTPUT);  
	pinMode(S1, OUTPUT);  
	pinMode(S2, OUTPUT);  
	pinMode(S3, OUTPUT);
	digitalWrite(S0, HIGH);  
	digitalWrite(S1, LOW);  
	s[0].setPins(S2, S3, Pino7_TCS1, 0);
	s[1].setPins(S2, S3, Pino7_TCS2, 1);
	s[2].setPins(S2, S3, Pino7_TCS3, 2);
	s[3].setPins(S2, S3, Pino7_TCS4, 3);
	for(i = 0; i < 4; i++)
		s[i].getFromEEPROM(); // It doesn't do anything at all, but it can be implemented.
	Serial.begin(9600);  

}  



    
void loop()
{
  	int i;
  	int cores_pegas[4];
  	for(i = 0; i < 4; i++)
  	{
  		cores_pegas[i] = s[i].WhatIsTheColor(); // The number gotten is 0, 1, 2 or 3.
  												// 0 = black
  												// 1 = white
  												// 2 = blue
  												// 3 = green
  		sensors[10+i] = "0" + cores_pegas[i];
  	}


}
