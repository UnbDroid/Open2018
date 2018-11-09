#include "lib.h"
#include <EEPROM.h>

int raiz(int q)
{
	int i;
	int p = q;
	for(i = 0; i < 3; i++)
		p -= (p*p-q)/(2*p);
	return p+1;
}

int maximo(int dr, int dg, int db)
{
	if(dr > dg && dr > db)
		return dr;
	if(dg > db)
		return dg;
	return db;
}




Color::Color()
{
}

Color::Color(int red, int green, int blue, int white)
{
	r = red;
	g = green;
	b = blue;
	w = white;
}



















void ColorInterval::set_med(Color m)
{
	this->med.r = m.r;
	this->med.g = m.g;
	this->med.b = m.b;
	this->med.w = m.w;
}

void ColorInterval::set_std(Color s)
{
	this->std.r = s.r;
	this->std.g = s.g;
	this->std.b = s.b;
	this->std.w = s.w;
}

int ColorInterval::distance(Color c)
{
	int dr = c.r, dg = c.g, db = c.b;
	dr -= this->med.r;
	dg -= this->med.g;
	db -= this->med.b;
	if(dr < 0) dr = -dr;
	if(dg < 0) dg = -dg;
	if(db < 0) db = -db;

	// dr /= this->std.r;
	// if(dr > 5)
	// 	dr = 5;
	// dg /= this->std.g;
	// if(dg > 5)
	// 	dg = 5;
	// db /= this->std.b;
	// if(db > 5)
	// 	db = 5;
	
	return maximo(dr, dg, db);
}



















// void TCS::setPins(const int S2, const int S3, const int OUT, const int sensor_number)
void TCS::setPins(const int S2, const int S3, const int OUT)
{
	this->S2 = S2;
	this->S3 = S3;
	this->OUT = OUT;
	pinMode(this->OUT, INPUT);
	
	// this->sensor_number = sensor_number; // It only will be used if we want to save the calibration on the EEPROM
}

int TCS::identify_color(Color cor_read) // returns 0, 1, 2 or 3. 0 = black, 1 = white, 2 = blue, 3 = green
{
	int i, min, soma;
	int dist[4];
	int medidos[4] = {1370, 350, 1300, 870};
	soma = cor_read.r + cor_read.g + cor_read.b + cor_read.w;
	for(i = 0; i < 4; i++)
	{
		dist[i] = soma - medidos[i];
		if(dist[i] < 0)
			dist[i] = -dist[i];
	}
	min = 0;
	for(i = 1; i < 4; i++)
		if(dist[i] < dist[min])
			min = i;

	
	// for(i = 0; i < 4; i++) // Because we have 4 possible colors on the arena: black, white, blue and green
	// {
	// 	dist[i] = c[i].distance(cor_read);
	// }
	// min = 0;
	// for(i = 1; i < 4; i++)
	// {
	// 	if(dist[i] < dist[min])
	// 		min = i;
	// }
	return min;
}

void TCS::setColorInterval(ColorInterval inter, int position)
{
	c[position].set_med(inter.med);
	c[position].set_std(inter.std);
}

Color TCS::read()
{
	Color newColor;
	int i;
	// int red = 1000, green = 1000, blue = 1000;
	int red, green, blue, white;
	digitalWrite(S2, LOW);
	digitalWrite(S3, LOW);
	// red = pulseIn(OUT, digitalRead(OUT) == HIGH ? LOW : HIGH);
	digitalWrite(S3, HIGH);  
	blue = pulseIn(OUT, digitalRead(OUT) == HIGH ? LOW : HIGH);  
	digitalWrite(S2, HIGH);  
	green = pulseIn(OUT, digitalRead(OUT) == HIGH ? LOW : HIGH); 
	// digitalWrite(S3, HIGH);
	// white = pulseIn(OUT, digitalRead(OUT) == HIGH ? LOW : HIGH); 
	// digitalWrite(S2, LOW);
	// digitalWrite(S3, LOW);
	newColor.r = 0;
	newColor.g = green;
	newColor.b = blue;
	newColor.w = 0;
	return newColor;
	
}


void TCS::calibrate(const int cor) // cor = 0, black; cor = 1, white; blue; green;
{
	// const int n_leituras = 10;
	// Color read_color;
	// ColorInterval intervalo;
	// int i, j;

	// int sum[3] = {0, 0, 0}; // sum of red, green and blue value ;
	// int sum2[3] = {0, 0, 0}; // quadratic sums to red, green and blue to calculate the std

	// for(j = 0; j < n_leituras; j++)
	// {
	// 	read_color = read();
	// 	sum[0] += read_color.r;
	// 	sum[1] += read_color.g;
	// 	sum[2] += read_color.b;
	// 	sum2[0] += read_color.r*read_color.r;
	// 	sum2[1] += read_color.g*read_color.g;
	// 	sum2[2] += read_color.b*read_color.b;
	// }
	// for(i = 0; i < 3; i++) // 3 because the sensor gets the red, green and blue colors
	// {
	// 	sum[i] /= n_leituras;
	// }
	// for(i = 0; i < 3; i++) // 3 because the sensor gets the red, green and blue colors
	// {
	// 	sum2[i] /= n_leituras;
	// 	sum2[i] -= sum[i]*sum[i];
	// 	sum2[i] = raiz(sum2[i]);
	// }
	// Serial.print("med:\t(");
	// Serial.print(sum[0]);
	// Serial.print(", ");
	// Serial.print(sum[1]);
	// Serial.print(", ");
	// Serial.print(sum[2]);
	// Serial.println(")");
	// Serial.print("std:\t");
	// Serial.print(sum2[0]);
	// Serial.print(", ");
	// Serial.print(sum2[1]);
	// Serial.print(", ");
	// Serial.print(sum2[2]);
	// Serial.println(")");
	
	// intervalo.set_med(Color(sum[0], sum[1], sum[2]));
	// intervalo.set_std(Color(sum2[0], sum2[1], sum2[2]));
	// setColorInterval(intervalo, cor);
}

void TCS::getFromEEPROM(const int start)
{
	// unsigned short int i;
	// for(i = 0; i < 4; i++) // There is 4 colors for each sensor: Black, white, green, blue;
	// {
	// 	c[i].med.r = EEPROM.read(start+6*i);
	// 	c[i].std.r = EEPROM.read(start+6*i+1);
	// 	c[i].med.g = EEPROM.read(start+6*i+2);
	// 	c[i].std.g = EEPROM.read(start+6*i+3);
	// 	c[i].med.b = EEPROM.read(start+6*i+4);
	// 	c[i].std.b = EEPROM.read(start+6*i+5);
	// }
}

void TCS::saveToEEPROM(const int start)
{
	// unsigned short int i;
	// for(i = 0; i < 4; i++) // There is 4 colors for each sensor: Black, white, green, blue;
	// {
	// 	EEPROM.write(start+6*i,		c[i].med.r);
	// 	EEPROM.write(start+6*i+1,	c[i].std.r);
	// 	EEPROM.write(start+6*i+2,	c[i].med.g);
	// 	EEPROM.write(start+6*i+3,	c[i].std.g);
	// 	EEPROM.write(start+6*i+4,	c[i].med.b);
	// 	EEPROM.write(start+6*i+5,	c[i].std.b);
	// }
}


int TCS::WhatIsTheColor()
{
	return identify_color(read());
}























LDR::LDR()
{
	this->p = 100;
	this->x = 0;
}
int LDR::identify_color(const int valor)
{
	int i, min;
	int dist[3];
	for(i = 0; i < 3; i++) // Because we have 4 possible colors on the arena: black, white, blue and green
	{
		dist[i] = valor-med_calib[i];
		if(dist[i] < 0) dist[i] = -dist[i];
	}
	min = 0;
	for(i = 1; i < 3; i++)
	{
		if(dist[i] < dist[min])
			min = i;
	}
	return min;
}
int LDR::read()
{
	unsigned int valor = analogRead(porta);
	int alpha = 4;
	x = (x*alpha + (10-alpha)*valor)/10;
	return x;

}
void LDR::calibrate(const int cor)
{
	int sum, sum2;
	int med = 0, std = 0;
	int lido;
	int i;
	const int n_leituras = 20;
	for(i = 0; i < n_leituras; i++)
	{
		lido = read();
		sum  += lido;
		sum2 += lido*lido;
	}
	med = sum/n_leituras;
	std = raiz(sum2/n_leituras - med*med);
	this->med_calib[cor] = med;
	this->std_calib[cor] = std;	
}
void LDR::setPorta(const int porta)
{
	this->porta = porta;
}
void LDR::set_limiar(const int limiar)
{
	this->limiar = limiar;
}
int LDR::WhatIsTheColor()
{
	return identify_color(read());
}

void LDR::set_qr(const int q, const int r)
{
	this->q = q;
	this->r = r;
}

void LDR::getFromEEPROM(const int start)
{
	unsigned short int i;
	for(i = 0; i < 3; i++)
	{
		med_calib[i] = EEPROM.read(4*i);
		std_calib[i] = EEPROM.read(4*i+2);
	}
}
void LDR::saveToEEPROM(const int start)
{
	unsigned short int i;
	for(i = 0; i < 3; i++)
	{
		EEPROM.write(start+2*i,		med_calib[i]);
		EEPROM.write(start+2*i+1,	std_calib[i]);
	}
}
