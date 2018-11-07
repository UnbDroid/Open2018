// Slave
//os ldrs sao as casas 2,3,4,5,6,7,8,9 deste vetor
//os sensores de cor sao as casas 10,11,12,13
//o vetor de comunicacao comeca com o byte I e termina com o byte F
#include "EEPROM.h"
#include "lib.h"

#define Pino1_TCS 7 	// S0 // Digital 7
#define Pino2_TCS 4		// S1 // Digital 4
#define Pino3_TCS 8		// S2 // Digital 8
// Pino4_TCS GND		// GND
#define Pino5_TCS 2		// S3 // Digital 2

#define Pino7_TCS1 3	// 1st sensor output // Digital3
#define Pino7_TCS2 9	// 2nd sensor output // Digital9
#define Pino7_TCS3 6	// 3th sensor output // Digital6
#define Pino7_TCS4 5	// 4th sensor output // Digital5

#define S0 Pino1_TCS
#define S1 Pino2_TCS
#define S2 Pino3_TCS
#define S3 Pino5_TCS

#define BLCK 0
#define WHITE 1
#define GREEN 2
#define BLUE 3

TCS s[4];
LDR l[8];

int i, j;
int cores_pegas[4];
int LDR_color[8];
Color cor_lida[4];
int ldr_lido;
int lidos[8], comFiltro[8];
int portas[8] = {A0, A1, A2, A3, A4, A5, A6, A7};

// Ordem
// A4, A2, A3, A0, A1, A7, A5, A6

void setup (void)
{
	// have to send on master in, *slave out*
	pinMode(MISO, OUTPUT);
	// turn on SPI in slave mode
	SPCR |= bit(SPE);

	// turn on interrupts
	SPCR |= bit(SPIE);

	// Definicoes dos sensores de cor
	pinMode(S0, OUTPUT);  
	pinMode(S1, OUTPUT);  
	pinMode(S2, OUTPUT);  
	pinMode(S3, OUTPUT);
	digitalWrite(S0, HIGH);  
	digitalWrite(S1, LOW);  
	s[0].setPins(S2, S3, Pino7_TCS1);
	s[1].setPins(S2, S3, Pino7_TCS2);
	s[2].setPins(S2, S3, Pino7_TCS3);
	s[3].setPins(S2, S3, Pino7_TCS4);

	// Definicao dos LDRs
	l[0].setPorta(A0);
	l[1].setPorta(A1);
	l[2].setPorta(A2);
	l[3].setPorta(A3);
	l[4].setPorta(A4);
	l[5].setPorta(A5);
	l[6].setPorta(A6);
	l[7].setPorta(A7);
	for(i = 0; i < 8; i++)
	{
		l[i].set_qr(1, 1000);
		
	}
    Serial.begin(9600);
}


void print_color(int n)
{
	if(n == 0)
		Serial.print("Black");
	else if(n == 1)
		Serial.print("White");
	else if(n == 2)
		Serial.print("Green");
	else
		Serial.print("Blue");
}


void loop (void)
{
	Serial.println("Iniciando a calibracao");
	delay(5000);
	// The number of the colors are BLACK = 0, WHITE = 1, GREEN = 2, BLUE = 3;
	for(i = 0; i < 4; i++)
	{
		// The number of the sensors are 4
		for(j = 0; j < 4; j++)
		{
			Serial.print("Put the color ");
			print_color(i);
			Serial.print(" on the sensor ");
			Serial.println(j+1);
			delay(5000);
			Serial.print("Started the color ");
			print_color(i);
			Serial.print(" on the sensor ");
			Serial.print(j+1);
			Serial.println("...");
			s[j].calibrate(i); // We need to choose the sensor j, and calibrate the color i
			Serial.println("Finished");
			delay(1000);
		}
	}
	Serial.print("\n\n\n\n");
	Serial.println("Saving all the files");
	for(i = 0; i < 4; i++) // Because we have 4 color sensors
	{
		s[i].saveToEEPROM(24*i);
	}
	Serial.println("Terminada a calibracao, desligue o arduino.");
	delay(10000);
}
