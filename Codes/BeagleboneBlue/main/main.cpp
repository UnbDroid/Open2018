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


#define SLAVE_MODE	SPI_SLAVE_MODE_AUTO
#define BUS_MODE	SPI_MODE_0
#define SPI_SPEED	5000


vector<char> SpiComm(int channel, vector<char> dado)
{
	char* test_str = reinterpret_cast<char*>(dado.data());
	int bytes = 13;	// get number of bytes in test string
	uint8_t buf[bytes];		// read buffer
	int ret;			// return value
	int batata = channel;
	if(rc_spi_init_auto_slave(BUS, SLAVE, BUS_MODE, SPI_SPEED)){
		vector<char> vec = {'F'};
		return vec;
	}

	// attempt a string send/receive test
	printf("Sending  %d bytes: %s\n", bytes, test_str);
	ret = rc_spi_transfer(BUS, SLAVE, (uint8_t*)test_str, bytes, buf);
	if(ret<0){
		printf("send failed\n");
		rc_spi_close(SLAVE);
		vector<char> vec = {'F'};
		return vec;
	}
	else printf("Received %d bytes: %s\n",ret, buf);

	vector<char> vec(buf, buf+bytes);
	rc_spi_close(BUS);
	return vec;
}

int main () 
{
	vector<char> dados = {'2','3','4','5'};
	vector<char> retorno = SpiComm(1,dados);
	//cout << retorno<<endl;
	return 0;
}

