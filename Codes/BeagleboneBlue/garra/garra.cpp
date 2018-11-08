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
	#include <rc/gpio.h>
}

using namespace std;

#define PI 3.14159265f

// Chip number in the beaglebone
#define CHIP_NUMBER_1 1
#define CHIP_NUMBER_2 3

// Pin numbersin the respective chipset
// #define GPIO_pin0 P9_23
// #define GPIO_pin1 P9_28


#define GPIO_pin0 17
#define GPIO_pin1 17


int main(){
	int check = 0;
	int i=0;

	// Define pins in system as GPIO
	system("config-pin p9_23 gpio");
	system("config-pin p9_28 gpio");

	// Inicializa os 2 pinos GPIO do chip 1 como output (pino 4 e 6)
	if(rc_gpio_init(CHIP_NUMBER_1, GPIO_pin0, GPIOHANDLE_REQUEST_OUTPUT)){
		// Erro, nao foi possivel inicializar
		return 1;
	};
	if(rc_gpio_init(CHIP_NUMBER_2, GPIO_pin1, GPIOHANDLE_REQUEST_OUTPUT)){
		// Erro, nao foi possivel inicializar
		return 1;
	};

	for(i=0;i<10;i++){
		if(rc_gpio_set_value(CHIP_NUMBER_1, GPIO_pin0, 1)){
			// Erro, nao mandou o valor
			break;
		};
		if(rc_gpio_set_value(CHIP_NUMBER_2, GPIO_pin1, 0)){
			// Erro, nao mandou o valor
			break;
		};
		rc_usleep(1000000);
		if(rc_gpio_set_value(CHIP_NUMBER_1, GPIO_pin0, 0)){
			// Erro, nao mandou o valor
			break;
		};
		if(rc_gpio_set_value(CHIP_NUMBER_2, GPIO_pin1, 1)){
			// Erro, nao mandou o valor
			break;
		};	
		rc_usleep(1000000);
	}

	rc_gpio_cleanup(CHIP_NUMBER_1, GPIO_pin0);
	rc_gpio_cleanup(CHIP_NUMBER_2, GPIO_pin1);
};
