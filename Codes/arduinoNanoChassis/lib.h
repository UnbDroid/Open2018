#ifndef SAIDA_H
#define SAIDA_H
 
#include <Arduino.h>
// #include <EEPROM.h>

class Color
{
public:
	Color();
	Color(unsigned char re, unsigned char gr, unsigned char bl);
	unsigned char r, g, b;
};

class ColorInterval
{
public:
	Color med, std;
	void set_med(Color m);
	void set_std(Color s);
	int distance(Color c);

};

class Sensor
{
private:
	int S0, S1, S2, S3, OUT;
	int sensor_number; // This only will be used if we want to save the calibration on the EEPROM
	int identify_color(Color c);
	Color getColor();
public:
	ColorInterval c[4]; // black, white, blue, green;
	void setPins(int S0, int S1, int S2, int S3, int OUT, int sensor_number);
	void setColorInterval(ColorInterval c, int position);
	void calibrate(int cor);
	void getFromEEPROM();
	void saveToEEPROM();
	int WhatIsTheColor();
};
 



#endif
