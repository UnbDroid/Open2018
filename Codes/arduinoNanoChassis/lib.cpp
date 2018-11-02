#include "lib.h"

int raiz(int q)
{
	int i;
	int p = q;
	for(i = 0; i < 3; i++)
		p -= (p*p-q)/(2*p);
	return p+1;
}

Color::Color()
{
}

Color::Color(unsigned char red, unsigned char green, unsigned char blue)
{
	r = red;
	g = green;
	b = blue;
}

void ColorInterval::set_med(Color m)
{
	this->med.r = m.r;
	this->med.g = m.g;
	this->med.b = m.b;
}

void ColorInterval::set_std(Color s)
{
	this->std.r = s.r;
	this->std.g = s.g;
	this->std.b = s.b;
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

	dr /= this->std.r;
	if(dr > 5)
		dr = 5;
	dg /= this->std.g;
	if(dg > 5)
		dg = 5;
	db /= this->std.b;
	if(db > 5)
		db = 5;
	
	return raiz(dr*dr + dg*dg + db*db);
}

void Sensor::setPins(int S0, int S1, int S2, int S3, int OUT, int sensor_number)
{
	this->S0 = S0;
	this->S1 = S1;
	this->S2 = S2;
	this->S3 = S3;
	this->OUT = OUT;
	this->sensor_number = sensor_number; // It only will be used if we want to save the calibration on the EEPROM
}

int Sensor::identify_color(Color cor)
{
	int i, min;
	int dist[4];
	for(i = 0; i < 4; i++) // Because we have 4 possible colors on the arena: black, white, blue and green
	{
		dist[i] = c[i].distance(cor);
	}
	min = 0;
	for(i = 1; i < 4; i++)
	{
		if(dist[i] < dist[min])
			min = i;
	}
	return min;
}

void Sensor::setColorInterval(ColorInterval inter, int position)
{
	c[position].set_med(inter.med);
	c[position].set_std(inter.std);
}

Color Sensor::getColor()
{
	Color newColor;
	int i;
	int red = 1000, green = 1000, blue = 1000;
	digitalWrite(S2, LOW);
	digitalWrite(S3, LOW);
	//count OUT, pRed, RED
	for(i = 0; i < 3 && red > 255 ; i++)
		red = pulseIn(OUT, digitalRead(OUT) == HIGH ? LOW : HIGH);
	if(i == 3)
		red = 0;
	digitalWrite(S3, HIGH);  
	//count OUT, pBLUE, BLUE  
	for(i = 0; i < 3 && blue > 255 ; i++)
		blue = pulseIn(OUT, digitalRead(OUT) == HIGH ? LOW : HIGH);  
	if(i == 3)
		blue = 0;
	digitalWrite(S2, HIGH);  
	//count OUT, pGreen, GREEN  
	for(i = 0; i < 3 && blue > 255 ; i++)
		green = pulseIn(OUT, digitalRead(OUT) == HIGH ? LOW : HIGH); 
	if(i == 3)
		green = 0;
	digitalWrite(S2, LOW);
	digitalWrite(S3, LOW);
}


void Sensor::calibrate(int cor) // cor = 0, black; cor = 1, white; blue; green;
{
	const int n_leituras = 10;
	Color read_color;
	ColorInterval intervalo;
	int i, j;

	int sum[3] = {0, 0, 0}; // sum of red, green and blue value ;
	int sum2[3] = {0, 0, 0}; // quadratic sums to red, green and blue to calculate the std

	for(j = 0; j < n_leituras; j++)
	{
		read_color = getColor();
		sum[0] += read_color.r;
		sum[1] += read_color.g;
		sum[2] += read_color.b;
		sum2[0] += read_color.r*read_color.r;
		sum2[1] += read_color.g*read_color.g;
		sum2[2] += read_color.b*read_color.b;
	}
	for(i = 0; i < 3; i++) // 3 because the sensor gets the red, green and blue colors
	{
		sum[i] /= n_leituras;
	}
	for(i = 0; i < 3; i++) // 3 because the sensor gets the red, green and blue colors
	{
		sum2[i] /= n_leituras;
		sum2[i] -= sum[i]*sum[i];
		sum2[i] = raiz(sum2[i]);
	}
	intervalo.set_med(Color(sum[0], sum[1], sum[2]));
	intervalo.set_std(Color(sum2[0], sum2[1], sum2[2]));
	setColorInterval(intervalo, cor);
}

void Sensor::getFromEEPROM()
{

}

void Sensor::saveToEEPROM()
{

}


int Sensor::WhatIsTheColor()
{
	return identify_color(getColor());
}