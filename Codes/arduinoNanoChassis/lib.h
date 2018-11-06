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

class TCS
{
private:
	int S2, S3, OUT;
	int sensor_number; // This only will be used if we want to save the calibration on the EEPROM
	void setColorInterval(ColorInterval c, int position);
	ColorInterval c[4]; // black, white, blue, green;
public:
	int identify_color(Color color_read);
	Color read();
	// void setPins(const int S2, const int S3, const int OUT, const int sensor_number);
	void setPins(const int S2, const int S3, const int OUT);
	void calibrate(const int cor);
	void getFromEEPROM();
	void saveToEEPROM();
	int WhatIsTheColor();
};

class LDR
{
private:
	int porta;
	int med_calib[3]; // White, Black, Green
	int std_calib[3];
	int x, p;
	int q, r;
public:
	LDR();
	void set_qr(const int q, const int r);
	int identify_color(const int value);
	int read();
	void setPorta(const int porta);
	void calibrate(const int cor);
	void getFromEEPROM();
	void saveToEEPROM();
	int WhatIsTheColor();	
};
 



#endif
