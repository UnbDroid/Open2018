/* 
 * Class written to deal with the drivers DRV8825 and A4988
 * Written specifically for the competition LARC-2018 by members of the team Droid-Open
 * Usable with 6-wire stepper motors, using only 4 of the wires in FULL bipolar configuration
 * Datasheets:
 *		DRV8825: http://www.ti.com/lit/ds/symlink/drv8825.pdf
 *		A4988:	 https://www.robotshop.com/media/files/pdf/datasheet-1182.pdf
 *
 * Pins: (Above view)
 *                    ___________________                          ___________________
 *              GND -|                   |- DIR              EN  -|                   |- VMOT
 *                   |    ___________    |                        |                   | 
 *              FLT -|   |           |   |- STP              MS1 -|                   |- GND
 *                   |   |  DRV8825  |   |                        |                   |
 *              2A  -|   |___________|   |- SLP              MS2 -|  _________        |- 2B
 *                   |                   |                        | |         |       |
 *              1A  -|                   |- RST              MS3 -| |         |       |- 2A           __  
 *                   |                   |                        | |  A4988  |       |              /  \ - TRIMPOT
 *              1B  -|                   |- M2               RST -| |         |       |- 1A          \__/
 *                   |                   |                        | |_________|       |
 *              2B  -|                   |- M1               SLP -|                   |- 1B 
 *                   |              __   |                        |         __        |                 
 *              GND -|             /  \  |- M0               STP -|        /  \       |- VDD              
 *                   |             \__/  |                        |        \__/       |                 
 *              VMOT-|___________________|- EN               DIR -|___________________|- GND
 * 
 *
 */

// Make sure the library is only called once
#ifndef Driver_h
#define Driver_h

class Driver{
	public:
		Driver();			// Constructor that sets DRV8825 as standard
		Driver(char*);		// Constructor that chooses between A4988 and DRV8825
		uint8_t Type();		// Just return the type, for debugging reasons
		void set_activation_pins(int, int, int);	// Choose activation pins (RESET, SLEEP, ENABLE)
		void set_step_and_dir_pins(int, int);		// Choose step and direction pins (STEP, DIR)
		void set_pins_resolution(int, int, int);	// Choose pins for resolution (M0, M1, M2)
		void start();				// Set all the pins as output
		void set_direction(int);	// Set direction standard: (1 - Clockwise, 0 - CounterClockwise)
		void set_resolution(int);	// Set resolution, (1-FULL, 2-HALF, 4-1/4 micro, 8-1/8 micro, 16-1/16 micro, 32-1/32(DRV8825))
		void send_pulses(int);		// Send a single number of pulses for the driver
		void set_speed(long int);	// Set speed in PPS (Pulses Per Second) or SPS (Steps Per Second), both are the same
		long int show_period();		// Show period for debugging reasons
		void set_axis(char);		// Set the axis that the driver will be responsible for controlling
		void set_diameter(float);	// Set the diameter to calculate conversion of rotational to translational movement, milimeters
		void move(char, int);		// Move the driver, absolute 'a' or relative 'r' values, order = (X, Y, Z, I, J, K), milimeters
		int calculate_pulses(float, float);		// Calculate number of pulses to make a certain movement, milimeters 
	private:
		uint8_t _type;		// Using 8 bits to represent driver, 0 for DRV8825, 1 for A4988
		uint8_t _RST;		// RESET pin
		uint8_t _SLP;		// SLEEP pin
		uint8_t _EN;		// ENABLE pin
		uint8_t _STP;		// STEP pin
		uint8_t _DIR;		// DIRECTION pin
		uint8_t _M0;		// pin for setting resolution
		uint8_t _M1;		// pin for setting resolution
		uint8_t _M2;		// pin for setting resolution
		long int _period;	// Time between steps, value in microseconds
		char _axis;			// Axis that the driver will be responsible for controlling
		float _pos;			// Position that the driver holds
		float _diameter;	// Outer diameter of what is connected to the stepper 
		float _resolution;	// Angular resolution, in degrees, that the driver will cause the stepper to rotate per step
};

int teste();

#endif
