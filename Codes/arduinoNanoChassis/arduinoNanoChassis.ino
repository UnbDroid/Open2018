// Slave
//os ldrs sao as casas 2,3,4,5,6,7,8,9 deste vetor
//os sensores de cor sao as casas 10,11,12,13
//o vetor de comunicacao comeca com o byte I e termina com o byte F
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

volatile byte sensors[17] = "II123113121121FF";

volatile int pos;
volatile bool active;

TCS s[4];
LDR l[8];

int i;
int cores_pegas[4];
int LDR_color[8];
Color cor_lida[4];
int ldr_lido;
int lidos[8], comFiltro[8];
int portas[8] = {A0, A2, A1, A3, A4, A5, A6, A7};


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
	l[1].setPorta(A2);
	l[2].setPorta(A1);
	l[3].setPorta(A3);
	l[4].setPorta(A4);
	l[5].setPorta(A5);
	l[6].setPorta(A6);
	l[7].setPorta(A7);
	for(i = 0; i < 8; i++)
	{
		l[i].set_qr(4, 1023);
	}
        Serial.begin(9600);
}




// SPI interrupt routine
ISR (SPI_STC_vect)
{
	byte c = SPDR;
	if (c == 's')
	{
		active = true;
		pos = 0;
		SPDR = sensors [pos++];   // send first byte
		//while (!(SPSR & (1 << SPIF)));
		return;
	}
	if (!active)
	{
		SPDR = 0;
		return;
	}
	SPDR = sensors [pos];
	if (sensors [pos] == 0 || ++pos >= sizeof (sensors))
		active = false;
}  // end of interrupt service routine (ISR) SPI_STC_vect


void imprime_LDRs()
{
	Serial.println("\n\n\n\n\n\n\n\n\n\n\n\n");
	Serial.println("LDR sem Filtro");
	for(i = 0; i < 8; i++)
	{
		Serial.print(i);
		Serial.print("\t");       
	}
	Serial.println(" ");
	for(i = 0; i < 8; i++)
	{
		Serial.print(lidos[i]);
		Serial.print("\t");
	}
	Serial.println(" ");
	Serial.println(" ");
	Serial.println("LDR com Filtro");
	for(i = 0; i < 8; i++)
	{
		Serial.print(i);
		Serial.print("\t");
	}
	Serial.println(" ");
	for(i = 0; i < 8; i++)
	{
		Serial.print(comFiltro[i]);
		Serial.print("\t");
	}
	Serial.println(" ");
}

void imprime_TCSs()
{
	int valor;
	Serial.println("\n\n\n\n\n\n\n");
	Serial.print("TCS\t");
	for(i = 0; i < 4; i++)
	{
		Serial.print(i);
		Serial.print("\t");
	}
	Serial.print("\nR\t");
	for(i = 0; i < 4; i++)
	{
		valor = cor_lida[i].r;
		Serial.print(valor);
		Serial.print("\t");
	}
	Serial.print("\nG\t");
	for(i = 0; i < 4; i++)
	{
		valor = cor_lida[i].g;	
		Serial.print(valor);
		Serial.print("\t");
	}
	Serial.print("\nB\t");
	for(i = 0; i < 4; i++)
	{
		valor = cor_lida[i].b;	
		Serial.print(valor);
		Serial.print("\t");
	}
}

void loop (void)
{
	// The number of the colors are BLACK = 0, WHITE = 1, GREEN = 2, BLUE = 3;
	for(i = 0; i < 8; i++)
	{
		lidos[i] = analogRead(portas[i]);
		comFiltro[i] = l[i].read();
		//LDR_color[i] = l[i].identify_color(ldr_lido);	// The number gotten is 0, 1 or 2. 
		//sensors[2+i] = '0' + LDR_color[i];
	}
	imprime_LDRs();
	for(i = 0; i < 4; i++)
	{
		cor_lida[i] = s[i].read();
		
		// cores_pegas[i] = s[i].identify_color(cor_lida[i]);	// The number gotten is 0, 1, 2 or 3.
		// sensors[10+i] = '0' + cores_pegas[i];
	}
	imprime_TCSs();
	delay(500);
}
