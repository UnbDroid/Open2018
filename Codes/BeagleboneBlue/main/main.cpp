#include <vector>
#include <iostream>
#include <fstream>

extern "C" {
    #include <stdio.h>
    #include <stdlib.h> // for atoi
	#include <getopt.h>
	#include <signal.h>
	#include <rc/mpu.h>
	#include <rc/math.h>
	#include <rc/button.h>
	#include <rc/encoder.h>
	#include <rc/time.h>
	#include <rc/motor.h>
	#include <rc/pthread.h>
	#include <rc/spi.h>
	#include <rc/servo.h>
	#include <rc/adc.h>
}

using namespace std;



#define PI 3.14159265f

// change these for your platform
// on BB this is equivilant to RC_BB_SPI1_SS1
#define BUS		1
#define SLAVE		0

#define CANAL_CHASSIS 0
#define CANAL_GARRA 1

#define BARRAMENTO_CHASSIS RC_BB_SPI1_SS1
#define BARRAMENTO_GARRA RC_BB_SPI1_SS2

#define SLAVE_MODE	SPI_SLAVE_MODE_AUTO
#define BUS_MODE	SPI_MODE_0
#define SPI_SPEED	2000


#define QUANTIDADE_LDR 8
#define QUANTIDADE_COR 4
#define TAMANHO_RESPOSTA_CHASSIS 16
#define SENSORES_LDR 1
#define SENSORES_COR 2
vector<uint8_t> sensoresLDR(8);
vector<uint8_t> sensoresCOR(4);


string SpiComm(int canal, char* dado, int tamanho_resposta);
void imprimeLeituras(int sensores);
void lerSensoresChassis();

int main () 
{
	//vector<char> dados = {'x','l'};
	//char* dados = "s";
	
	string retorno = SpiComm(0,"s",16);
	cout << retorno <<endl<<retorno.length()<<endl;
	lerSensoresChassis();
	imprimeLeituras(SENSORES_LDR);
	imprimeLeituras(SENSORES_COR);
	return 0;
}


string SpiComm(int canal, char* dado, int tamanho_resposta)
{
	char* test_str = dado;
	int bytes = tamanho_resposta;	// get number of bytes in test string
	uint8_t buf[bytes];		// read buffer
	int ret;			// return value
	// attempt a string send/receive test
	printf("Sending  %d bytes: %s\n", bytes, test_str);
	if(canal == CANAL_CHASSIS){
		if(rc_spi_init_auto_slave(BARRAMENTO_CHASSIS, BUS_MODE, SPI_SPEED)){
			string vec = {'F'};
			return vec;
		}
	
		ret = rc_spi_transfer(BARRAMENTO_CHASSIS, (uint8_t*)test_str, bytes, buf);
	}else if(canal == CANAL_GARRA){
		if(rc_spi_init_auto_slave(BARRAMENTO_GARRA, BUS_MODE, SPI_SPEED)){
			string vec = {'F'};
			return vec;
		}
		ret = rc_spi_transfer(BARRAMENTO_GARRA, (uint8_t*)test_str, bytes, buf);
	}
	if(ret<0){
		printf("send failed\n");
		rc_spi_close(canal);
		string vec = {'F'};
		return vec;
	}
	else printf("Received %d bytes: %s\n",ret, buf);
	string vec = string((char *)buf);
	rc_spi_close(1);
	return vec;
}

void lerSensoresChassis()
{
	//char* tag = "s";
	string aux = SpiComm(CANAL_CHASSIS,"s",TAMANHO_RESPOSTA_CHASSIS);
	int j = 0;
	for (int i = 0; (i < signed(aux.length())) && (j < (QUANTIDADE_LDR + QUANTIDADE_COR)); ++i)
	{
		if (aux[i] >= '1' && aux[i] <= ('1'+2))
		{
			if(j<QUANTIDADE_LDR){
				sensoresLDR[j] = aux[i] - '0';
				//printf("%c %d \n",aux[i],sensoresLDR[j]);
			}
			else{
				sensoresCOR[j- QUANTIDADE_LDR] = aux[i] - '0';
				//printf("%c %d j=%d \n",aux[i],sensoresCOR[j- QUANTIDADE_LDR],j);
			}
			j++;
		}
	}
}


void imprimeLeituras(int sensores)
{
	if (sensores == SENSORES_LDR)
	{
		for (int i = 0; i < signed(sensoresLDR.size()); ++i)
		{
			cout<<"sensorLDR "<<i<<"= "<<int(sensoresLDR[i])<<endl;
		}
	}else if(sensores == SENSORES_COR){
		for (int i = 0; i < signed(sensoresCOR.size()); ++i)
		{
			cout<<"sensorCOR "<<i<<"= "<<int(sensoresCOR[i])<<endl;
		}
	}
}

