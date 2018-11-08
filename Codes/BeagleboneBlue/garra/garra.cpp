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
#define CHIP_1 1
#define CHIP_2 3

// Pin numbers in the respective chipset
#define PIN_1 17
#define PIN_2 17

#define GPIO_pin0 17
#define GPIO_pin1 17

#define PERIOD 2000000 // In microseconds (2000000) = 2 seconds per step

static int running = 1;

// Deal with keyboard interrupt
[[gnu::unused]] static void __signal_handler([[gnu::unused]] int dummy){
	running = 0;
	return;
}

// Receives Step and Dir pins and sets them as outpu
bool setup_step_and_dir_pins(int chip1, int step, int chip2, int dir){
	// Inicializa os 2 pinos GPIO do chip 1 como output (pino 4 e 6)
	if(rc_gpio_init(chip1, step, GPIOHANDLE_REQUEST_OUTPUT)){
		// Erro, nao foi possivel inicializar
		return 1;
	};
	if(rc_gpio_init(chip2, dir, GPIOHANDLE_REQUEST_OUTPUT)){
		// Erro, nao foi possivel inicializar
		return 1;
	};	
	
	return 0;
}

// Clean the step and dir pins used during the code
void clean_all(int chip1, int step, int chip2, int dir){
	rc_gpio_cleanup(chip1, step);
	rc_gpio_cleanup(chip2, dir);	
}

// Choose rotation direction
bool set_direction(bool dir){
	if(rc_gpio_set_value(CHIP_2, PIN_2, dir))
		return 1;
		
	return 0;	
}

// Rise step signal
bool step_up(){
	if(rc_gpio_set_value(CHIP_1, PIN_1, 1))
		return 1;
		
	return 0;	
}

// Fall step signal
bool step_down(){
	if(rc_gpio_set_value(CHIP_1, PIN_1, 0))
		return 1;
		
	return 0;
}

// Send a number of pulses to the driver
bool send_pulses(int n_steps){
	// Codigo porco, deveria passar parametros, mas vou usar apenas os defines
	
	// Configura a direcao baseado no numero de steps passados
	if(n_steps<0){
		n_steps = -n_steps;
		if(set_direction(0)){	// Set direction negative, checking possible errors
			return 1;	// return erro, main cleans everything
		};
	}	
	else {
		if(set_direction(1)){	// Set direction positive, checking possible errors
			return 1;	// return erro, main cleans everything
		};		
	};
	
	// Start the step with 0
	if(step_down()){
		return 1; // Error
	};
	
	rc.usleep(100); // 100us for safety
	
	while(n_steps && running){
		// Send a pulse based on time
		if(step_up()){
			return 1;
		};
		rc.usleep(PERIOD/2);
		if(step_down()){
			return 1;
		};
		rc.usleep(PERIOD/2);
	};
	
	return 0;	// Everything alright
}

int main(){
	int i=0;

	// Define pins in system as GPIO
	system("config-pin p9_23 gpio");
	system("config-pin p9_28 gpio");

	// Inicializa os 2 pinos GPIO do chip 1 como output (pino 4 e 6)
	if(setup_step_and_dir_pins(CHIP_1, PIN_1, CHIP_2, PIN_2)){
		// If got here, an error ocurred, clean and exit
		clean_all(CHIP_1, PIN_1, CHIP_2, PIN_2);
		return 1;
	};

	if(send_pulses(10)){
		clean_all(CHIP_1, PIN_1, CHIP_2, PIN_2);
		return 1;
	};

	if(send_pulses(-10)){
		clean_all(CHIP_1, PIN_1, CHIP_2, PIN_2);
		return 1;
	};	

	clean_all(CHIP_1, PIN_1, CHIP_2, PIN_2);
	return 0;
};
