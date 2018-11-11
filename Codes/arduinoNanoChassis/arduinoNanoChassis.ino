//// Slave
////os ldrs sao as casas 2,3,4,5,6,7,8,9 deste vetor
////os sensores de cor sao as casas 10,11,12,13
////o vetor de comunicacao comeca com o byte I e termina com o byte F
//#include "EEPROM.h"
//#include "lib.h"
//
//#define Pino1_TCS 7 	// S0 // Digital 7
//#define Pino2_TCS 4		// S1 // Digital 4
//#define Pino3_TCS 8		// S2 // Digital 8
//// Pino4_TCS GND		// GND
//#define Pino5_TCS 2		// S3 // Digital 2
//
//#define Pino7_TCS1 3	// 1st sensor output // Digital3
//#define Pino7_TCS2 9	// 2nd sensor output // Digital9
//#define Pino7_TCS3 6	// 3th sensor output // Digital6
//#define Pino7_TCS4 5	// 4th sensor output // Digital5
//
//#define S0 Pino1_TCS
//#define S1 Pino2_TCS
//#define S2 Pino3_TCS
//#define S3 Pino5_TCS
//
//// 			Os limiares
////   | White | Black | Green |
//// 0 |  130  |  360  |  390  |
//// 1 |  180  |  440  |  370  |
//// 2 |  370  |  725  |  720  |
//// 3 |  180  |  525  |  500  |
//// 4 |  170  |  420  |  380  |
//// 5 |  300  |  700  |  680  |
//// 6 |  120  |  250  |  240  |
//// 7 |  130  |  310  |  340  |
//
//#define Limit1 (130+360)/2//
//#define Limit2 (180+440)/2//
//#define Limit3 (370+725)/2//
//#define Limit4 (180+525)/2//
//#define Limit5 (170+420)/2//
//#define Limit6 (300+700)/2//
//#define Limit7 (120+250)/2//
//#define Limit8 (130+310)/2//
//
//#define BLACK 0
//#define WHITE 1
//#define GREEN 2
//#define BLUE 3
//
//volatile byte sensors[17] = "II123113121121FF";
//
//volatile int pos;
//volatile bool active; 
//
//TCS s[4];
//LDR l[8];
//
//int i, j;
//int cores_pegas[4]; // Para armazenar os valores de 0 a 3 do sensor de cor
//Color cor_lida[4];  // Para armazenar o objeto cor recem lido. Nao eh necessario, mas assim armazena-se o valor
//
//int LDR_color[8];   // Para armazenar os valores de 0 a 2 dos LDRs centrais
//int ldr_lido[8];    // Para armazenar o valor lido do LDR, podendo assumir o valor de centenas.
//// int portas[8] = {A0, A1, A2, A3, A4, A5, A6, A7};
//const int Limit[8] = {Limit1, Limit2, Limit3, Limit4, Limit5, Limit6, Limit7, Limit8};
//const int portas[8] = {A0, A1, A7, A5, A6, A4, A2, A3};
//
//// Ordem, olhando de cima do robo com a placa montada
////                  Frente
////           A1 = 2     A0 = 1
////          (Limit2)   (Limit1)
////                                     
////   A7 = 3                     A3 = 8
////  (Limit3)                   (Limit8)
////                                       
////   A5 = 4                     A2 = 7
////  (Limit4)                   (Limit7)
////                                     
////           A6 = 5     A4 = 6
////          (Limit5)   (Limit6)
////                 Tras 
//// A0, A1, A7, A5, A6, A4, A2, A3
//
//void setup (void)
//{
//	// have to send on master in, *slave out*
//	pinMode(MISO, OUTPUT);
//	// turn on SPI in slave mode
//	SPCR |= bit(SPE);
//
//	// turn on interrupts
//	SPCR |= bit(SPIE);
//
//	// Definicoes dos sensores de cor
//	pinMode(S0, OUTPUT);  
//	pinMode(S1, OUTPUT);  
//	pinMode(S2, OUTPUT);  
//	pinMode(S3, OUTPUT);
//	digitalWrite(S0, HIGH);  
//	digitalWrite(S1, LOW);  
//	s[0].setPins(S2, S3, Pino7_TCS1);
//	s[1].setPins(S2, S3, Pino7_TCS2);
//	s[2].setPins(S2, S3, Pino7_TCS3);
//	s[3].setPins(S2, S3, Pino7_TCS4);
//
//	// k.setPins(S2, S3, OUT);
//	// Definicao dos LDRs
//	
//	for(i = 0; i < 8; i++)
//	{
//		l[i].setPorta(portas[i]);
//		l[i].set_qr(1, 1000);
//		
//	}
//	for(i = 0; i < 4; i++) // Capturar os 4 sensores de cor
//	{
//		s[i].getFromEEPROM(24*i);	
//	}
//    Serial.begin(9600);
//    // imprime_tudo_pego();
//    // delay(15000);
//}
//
//// void imprime_tudo_pego() // Chamar no setup logo depois de pegar os dados pelo EEPROM
//// {
//// 	int i, j;
//// 	for(i = 0; i < 4; i++) // 4 sensores
//// 	{
//// 		for(j = 0; j < 4; j++) // 4 cores no chao
//// 		{
//// 			Serial.print("Sensor ");
//// 			Serial.print(i);
//// 			Serial.print(" cor ");
//// 			print_color(j);
//// 			Serial.println();
//// 			Serial.print("med: (");
//// 			Serial.print(s[i].c[j].med.r);
//// 			Serial.print(", ");
//// 			Serial.print(s[i].c[j].med.g);
//// 			Serial.print(", ");
//// 			Serial.print(s[i].c[j].med.b);
//// 			Serial.println(")");		
//// 			Serial.print("std: (");
//// 			Serial.print(s[i].c[j].std.r);
//// 			Serial.print(", ");
//// 			Serial.print(s[i].c[j].std.g);
//// 			Serial.print(", ");
//// 			Serial.print(s[i].c[j].std.b);
//// 			Serial.println(")");			
//// 		}
//// 	}
//// }
//
//
//// SPI interrupt routine
//ISR (SPI_STC_vect)
//{
//	byte c = SPDR;
//	if (c == 's')
//	{
//		active = true;
//		pos = 0;
//		SPDR = sensors [pos++];   // send first byte
//		//while (!(SPSR & (1 << SPIF)));
//		return;
//	}
//	if (!active)
//	{
//		SPDR = 0;
//		return;
//	}
//	SPDR = sensors [pos];
//	if (sensors [pos] == 0 || ++pos >= sizeof (sensors))
//		active = false;
//}  // end of interrupt service routine (ISR) SPI_STC_vect
//
//
//// void imprime_LDRs()
//// {
//// 	Serial.println("!!clear!!");
//// 	Serial.println("LDR sem Filtro");
//// 	for(i = 0; i < 8; i++)
//// 	{
//// 		Serial.print(i);
//// 		Serial.print("\t");       
//// 	}
//// 	Serial.println(" ");
//// 	for(i = 0; i < 8; i++)
//// 	{
//// 		Serial.print(ldr_lido[i]);
//// 		Serial.print("\t");
//// 	}
//// 	Serial.println(" ");
//// 	Serial.println(" ");
//
//
//// 	Serial.println("Cores:");
//// 	Serial.print("LDR\t");
//// 	for(i = 0; i < 8; i++)
//// 	{
//// 		if(ldr_lido[i] > Limit[i])
//// 			print_color(0);
//// 		else
//// 			print_color(1);
//// 		Serial.print('\t');
//// 	}
//// 	Serial.println("");
//// 	// Serial.println("LDR com Filtro");
//// 	// for(i = 0; i < 8; i++)
//// 	// {
//// 	// 	Serial.print(i);
//// 	// 	Serial.print("\t");
//// 	// }
//// 	// Serial.println(" ");
//// 	// for(i = 0; i < 8; i++)
//// 	// {
//// 	// 	Serial.print(comFiltro[i]);
//// 	// 	Serial.print("\t");
//// 	// }
//// 	// Serial.println(" ");
//// }
//
//// void imprime_TCSs()
//// {
//// 	int valor;
//// 	Serial.println("!!clear!!");
//// 	Serial.print("TCS\t");
//// 	for(i = 0; i < 4; i++)
//// 	{
//// 		Serial.print(i);
//// 		Serial.print("\t");
//// 	}
//// 	Serial.print("\nR\t");
//// 	for(i = 0; i < 4; i++)
//// 	{
//// 		valor = cor_lida[i].r;
//// 		Serial.print(valor);
//// 		Serial.print("\t");
//// 	}
//// 	Serial.print("\nG\t");
//// 	for(i = 0; i < 4; i++)
//// 	{
//// 		valor = cor_lida[i].g;	
//// 		Serial.print(valor);
//// 		Serial.print("\t");
//// 	}
//// 	Serial.print("\nB\t");
//// 	for(i = 0; i < 4; i++)
//// 	{
//// 		valor = cor_lida[i].b;	
//// 		Serial.print(valor);
//// 		Serial.print("\t");
//// 	}
//// }
//
//// void print_color(int n)
//// {
//// 	if(n == 0)
//// 		Serial.print("Black");
//// 	else if(n == 1)
//// 		Serial.print("White");
//// 	else if(n == 2)
//// 		Serial.print("Green");
//// 	else
//// 		Serial.print("Blue");
//// }
//
//// void imprime_tratados()
//// {
//// 	Serial.println("TCS\t1\t2\t3\t4");
//// 	Serial.print("\t");
//// 	for(i = 0; i < 4; i++)
//// 	{
//// 		print_color(cores_pegas[i]);
//// 		Serial.print('\t');
//// 	}
//// 	Serial.println("");
//// }
//
//// void imprime_medias()
//// {
//// 	int valor, j;
//// 	Serial.println("TCS\t1\t2\t3\t4");
//// 	for(i = 0; i < 4; i++)
//// 	{
//// 		if(i == 0)
//// 			Serial.print("BK\t");
//// 		else if(i == 1)
//// 			Serial.print("WH\t");
//// 		else if(i == 2)
//// 			Serial.print("GR\t");
//// 		else
//// 			Serial.print("BU\t");
//
//// 		for(j = 0; j < 4; j++)
//// 		{
//// 			valor = s[i].c[j].distance(cor_lida[i]);
//// 			Serial.print(valor);
//// 			Serial.print("\t");
//// 		}
//// 		Serial.println("");
//// 	}
//	
//// }
//
//void loop (void)
//{
//	// The number of the colors are BLACK = 0, WHITE = 1, GREEN = 2, BLUE = 3;
//	// Color we;
//	
//	for(i = 0; i < 8; i++)
//	{
//		ldr_lido[i] = l[i].read();
//		LDR_color[i] = l[i].identify_color(ldr_lido[i]);	// The number gotten is 0, 1 or 2. 
//		sensors[2+i] = '0' + LDR_color[i];
//	}
//	// imprime_LDRs();
//	for(i = 0; i < 4; i++)
//	{
//		cor_lida[i] = s[i].read();
//		// we = k.read();
//		cores_pegas[i] = s[i].identify_color(cor_lida[i]);	// The number gotten is 0, 1, 2 or 3.
//		sensors[10+i] = '0' + cores_pegas[i];
//	}
//	for(i = 0; i < 17; i++)
//		Serial.print(char(sensors[i]));
//    Serial.println("");
//	// Serial.print("RGB: (");
//	// Serial.print(we.r);
//	// Serial.print(", ");
//	// Serial.print(we.g);
//	// Serial.print(", ");
//	// Serial.print(we.b);
//	// Serial.print(", ");
//	// Serial.print(we.w);
//	// Serial.println(")");
//	// imprime_TCSs();
//	// Serial.println("\n");
//	// imprime_tratados();
//	// Serial.println("\n");
//	// imprime_medias();
//}
