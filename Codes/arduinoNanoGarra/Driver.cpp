#include "Arduino.h"
#include "Driver.h"

// The next 2 functions were written just to avoid calling other libraries

// Just return the upper_case of a char
char upper(char c){
	if(c >= 'a' && c<='z'){
		return (c - 'a' + 'A');
	}
	return c;
}

int cmp_str(char* a, char* b){
	int i;
	for(i=0;i<8;i++){					// 7 chars limit = DRV8825
		if(upper(a[i]) != upper(b[i])){
			break;
		}
		else {
			if(a[i] == '\0'){
				return 1;
			};
		};
	};
	return 0;
}

// Function to verify if pin is in valid range
int is_set(uint8_t pin){
	if(pin>=0 && pin<14){
		return 1;
	};
	return 0;
}

// Check if pin is output enabled
int output_enabled(uint8_t pin){
	if(pin<=7){
		if(DDRD & (1 << pin)){		// Check if it is set as OUTPUT, ports 0 to 7
			return 1;		
		}; 
	}
	else {
		if(DDRB & (1 << (pin-8))){		// Check if it is set as OUTPUT, ports 8 to 13
			return 1;
		};
	};
	return 0;	
}

// Fist constructor, no parameters, define the driver as "DRV8825"
Driver::Driver(){
	this->_type = 0;	// Set as Default, 0 equals DRV8825	
	this->_period = 4;	// 3.8 us minimum needed according to datasheet
	this->_axis = 'X';	// Set axis as x
	this->_pos = 0;
	this->_diameter = 1;
}

// Second constructor, string parameter
Driver::Driver(char* str){
	if(cmp_str(str, "A4988")){		// If the user chooses A4988
		this->_type = 1;
		this->_period = 3;	// 2 us minimum needed according to datasheet
	}
	else {							// The only other option is DRV8225, ignoring 'str'
		this->_type = 0;
		this->_period = 4;	// 3.8 us minimum needed according to datasheet
	};
	this->_axis = 'X';
	this->_pos = 0;
	this->_diameter = 1;
}

// Simply return the type, for debugging
uint8_t Driver::Type(){
	return this->_type;
}

void Driver::set_activation_pins(int RST, int SLP, int EN){
	if(RST>=0 && RST<14){
		this->_RST = uint8_t(RST);
	}
	else{
		this->_RST = 255;	// Equivalent to -1 in int, not set, assuming the user pulled it to ground or VCC
	};

	if(SLP>=0 && SLP<14){	
		this->_SLP = uint8_t(SLP);
	}
	else{
		this->_SLP = 255;	// Equivalent to -1 in int, not set, assuming the user pulled it to ground or VCC
	};
	
	if(EN>=0 && EN<14){	
		this->_EN = uint8_t(EN);
	}
	else{
		this->_EN = 255;	// Equivalent to -1 in int, not set, assuming the user pulled it to ground or VCC
	};	
}

void Driver::set_step_and_dir_pins(int STP, int DIR){
	if(STP>=0 && STP<14){
		this->_STP = uint8_t(STP);
	}
	else{
		this->_STP = 255;
	};
	
	if(DIR>=0 && DIR<14){
		this->_DIR = uint8_t(DIR);
	}
	else{
		this->_DIR = 255;
	};	
}

void Driver::set_pins_resolution(int M0, int M1, int M2){
	if(M0>=0 && M0<14){
		this->_M0 = uint8_t(M0);
	}
	else{
		this->_M0 = 255;
	};
	
	if(M1>=0 && M1<14){
		this->_M1 = uint8_t(M1);
	}
	else{
		this->_M1 = 255;
	};	
	
	if(M2>=0 && M2<14){
		this->_M2 = uint8_t(M2);
	}
	else{
		this->_M2 = 255;
	};			
}

void Driver::start(){
	// First define the activation pins as output
	if(is_set(this->_RST)){
		pinMode(this->_RST, OUTPUT);
	};
	if(is_set(this->_SLP)){
		pinMode(this->_SLP, OUTPUT);
	};
	if(is_set(this->_EN)){
		pinMode(this->_EN, OUTPUT);
	};
	
	// Then define the resolution pins as output
	if(is_set(this->_M0)){
		pinMode(this->_M0, OUTPUT);
	};
	if(is_set(this->_M1)){
		pinMode(this->_M1, OUTPUT);
	};
	if(is_set(this->_M2)){
		pinMode(this->_M2, OUTPUT);
	};
	
	// Finally, set the STP and DIR pins as output
	if(is_set(this->_STP)){
		pinMode(this->_STP, OUTPUT);
	};
	if(is_set(this->_DIR)){
		pinMode(this->_DIR, OUTPUT);
	};					
}

void Driver::set_direction(int dir){
	if(!dir){
		if(is_set(this->_DIR)){
			if(output_enabled(this->_DIR)){
				digitalWrite(this->_DIR, LOW);
			};
		};
	}
	else {
		if(is_set(this->_DIR)){
			if(output_enabled(this->_DIR)){
				digitalWrite(this->_DIR, HIGH);
			};
		};	
	};
}

void Driver::set_resolution(int res){
	// This function also allows the user to set a manual resolution, in case the M0, M1 and M2 pins are hardwired
	int verification = 1;

	// First check if the 3 pins for the resolution are valid and set as OUTPUT
	if(!is_set(this->_M0) || !is_set(this->_M1) || !is_set(this->_M2)){
		verification = 0;
	};
	if(!output_enabled(this->_M0) || !output_enabled(this->_M1) || !output_enabled(this->_M2)){
		verification = 0;
	};
	
	// Assuming 200 steps per revolution
	switch(res){
		case 1:			// Same for DRV8825 and A4988, FULL step
			if(verification){
				digitalWrite(this->_M0, LOW);
				digitalWrite(this->_M1, LOW);
				digitalWrite(this->_M2, LOW);
			};
			this->_resolution = (float)360/200;
			break;
		case 2:			// Same for DRV8825 and A4988, HALF step
			if(verification){
				digitalWrite(this->_M0, HIGH);
				digitalWrite(this->_M1, LOW);
				digitalWrite(this->_M2, LOW);
			};
			this->_resolution = (float)360/(2*200);
			break;
		case 4:			// Same for DRV8825 and A4988, 1/4 microstep
			if(verification){
				digitalWrite(this->_M0, LOW);
				digitalWrite(this->_M1, HIGH);
				digitalWrite(this->_M2, LOW);
			};
			this->_resolution = (float)360/(4*200);
			break;
		case 8:			// Same for DRV8825 and A4988, 1/8 microstep
			if(verification){
				digitalWrite(this->_M0, HIGH);
				digitalWrite(this->_M1, HIGH);
				digitalWrite(this->_M2, LOW);
			};
			this->_resolution = (float)360/(8*200);
			break;
		case 16:		// 1/16 microstep
			if(verification){
				digitalWrite(this->_M0, this->_type);	// HIGH for A4988, LOW otherwise
				digitalWrite(this->_M1, this->_type);	// HIGH for A4988, LOW otherwise
				digitalWrite(this->_M2, HIGH);
			};
			this->_resolution = (float)360/(16*200);
			break;
		case 32:		// 1/32 microstep, for A4988 deal like 1/16
			if(verification){
				digitalWrite(this->_M0, HIGH);
				digitalWrite(this->_M1, this->_type);
				digitalWrite(this->_M2, HIGH);
			};
			if(this->_type){
				this->_resolution = (float)360/(16*200);
			}	
			else {
				this->_resolution = (float)360/(32*200);
			};
			break;
		default:		// Default is FULL step
			if(verification){
				digitalWrite(this->_M0, LOW);
				digitalWrite(this->_M1, LOW);
				digitalWrite(this->_M2, LOW);
			};		
			this->_resolution = (float)360/200;
			break;					
	};	
}

void Driver::send_pulses(int steps){
	// Check if is valid and output enabled
	if(!is_set(this->_STP) || !output_enabled(this->_STP))
		return;
	
	// The next line might not even be necessary, but it does not hurt to be secure
	delayMicroseconds(1);	// 200ns for A4988 and 650ns for DRV8825, for security reasons, use 1000ns
	
	// For the DRV8225, it would be best to actually wait 1.7 ms, if it just got out of SLEEP mode, this could be implemented later
		
	// Make sure step is initially low	
	digitalWrite(this->_STP, LOW);
	
	delayMicroseconds((this->_period)/2);	// Wait before rising STP output again	
		
	int i;
	for(i=0;i<steps;i++){
		digitalWrite(this->_STP, HIGH);
		delayMicroseconds((this->_period)/2);
		digitalWrite(this->_STP, LOW);
		delayMicroseconds((this->_period)/2);
	};
}

void Driver::set_speed(long int SPS){		// Speed input as Steps Per Second
	// It is important to note that if the motor does not work as expected the problem could be with the motor, and not the driver
	this->_period = 1000000 / SPS;		//	(microseconds/second) / (steps/second) = (microseconds/steps)
	
	if(this->_type){		// If A4988
		if(this->_period < 3){
			this->_period = 3;
		};
	}
	else {
		if(this->_period < 4){
			this->_period = 4;
		};
	};
}

long int Driver::show_period(){
	return this->_period;
}

void Driver::set_axis(char c){
	c = upper(c);

	// X, Y, Z cartesian axis, I, J, K rotational axis	
	switch(c){
		case 'X':
			this->_axis = 'X';
			break;
		case 'Y':
			this->_axis = 'Y';
			break;
		case 'Z':
			this->_axis = 'Z';
			break;
		case 'I':
			this->_axis = 'I';
			break;
		case 'J':
			this->_axis = 'J';
			break;
		case 'K':
			this->_axis = 'K';
			break;
		default:
			this->_axis = 'X';
			break;						
	};
}

void Driver::set_diameter(float diameter){
	this->_diameter = diameter;
}

int Driver::calculate_pulses(float orig, float dest){	
	return ((dest-orig)/(this->_diameter/2)*(180/3.1415926535))/this->_resolution; 	
}

void Driver::move(char mov, int dist=0){
	int n_pulses = 0;
	
	if(mov == 'a'){
		// Absolute movement, goes to specific position
		n_pulses = this->calculate_pulses(this->_pos, dist);
		this->_pos = dist;
	}
	else {
		// Relative movement
		n_pulses = this->calculate_pulses(0, dist);
		this->_pos += dist;
	};
	
	if(n_pulses<0){
		this->set_direction(0);
		n_pulses = -n_pulses;
	}	
	else {
		this->set_direction(1);
	};
	
	this->send_pulses(n_pulses);	
}

int teste(){
	return 1;
}
