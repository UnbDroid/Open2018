/* Begin includes */

#include <Driver.h>

/* End includes */

/* Defines for SPI communication */

#define ID_SENSORES_US (char *)"u"        // -> UltraSonic
#define ID_SENSOR_COR_GARRA (char *)"p"   // -> Color sensor
#define ID_LIGA_ELETROIMA (char *)"e"     // -> Turn on Electromagnets
#define ID_DESLIGA_ELETROIMA (char *)"d"  // -> Turn off Electromagnets
#define ID_MEXE_GARRA_Z (char *)"z "      // -> 'Z' axis movement
#define ID_MEXE_GARRA_R (char *)"r "      // -> 'R' radial movement
#define ID_MEXE_GARRA_2 (char *)"b  "      // -> Both axis simultaneously
#define ID_CHECA_GARRA (char *)"c"        // -> Check if any motor is moving

/* Defines for reduction of readable code */

#define RETURN_SPI_SUCCESS ans[2] = '1'; active = true; pos = 0; SPDR = ans[pos++]; return;
#define RETURN_SPI_FAIL ans[2] = '0'; active = true; pos = 0; SPDR = ans[pos++]; return;

/* Defines for SPI handshake */

// CHAVE_STRT CHAVE_STRT DADO ... CHAVE_FIM CHAVE_FIM
#define HANDSHAKE_INICIO_US (unsigned char) 253
#define HANDSHAKE_FIM_US (unsigned char) 254

#define HANDSHAKE_INICIO_GARRA (unsigned char) 253
#define HANDSHAKE_FIM_GARRA (unsigned char) 254

/* Defines for stepper motor */

#define MAXIMUM_Z (float)110   // Maximum movement for z axis                   <<<<-------- THIS NEEDS CHECKING
#define MAXIMUM_R (float)118   // Maximum movement for radial axis
#define DIAMETER_Z (float)29.4  
#define DIAMETER_R (float)11

/* Defines for eletromagnets */

#define Eletro_1 5
#define Eletro_2 6

/* End defines*/

/* Start driver variables */

Driver z_motor("A4988");
Driver r_motor("A4988");

volatile int pos;
volatile bool active;

volatile bool check_movement;

volatile bool waiting_second_byte = 0;
volatile byte last_byte;
volatile byte third_byte;

volatile float distance_1;
volatile float distance_2;

volatile bool turn_on_magnet = 0;
volatile bool turn_off_magnet = 0;

// Vetor para responder o BBB
volatile byte ans[5] = "II FF";

void setup (void){
  /* SPI setup */
  
  // have to send on master in, *slave out*
  pinMode(MISO, OUTPUT);
  
  // SPI Bitwise setup
  // Enable SPI
  SPCR |= bit(SPE);
  
  // Turn on SPI interrupts
  SPCR |= bit(SPIE);

  // Bits DORD = 0 (MSB first), MSTR = 0 (Arduino as slave), CPOL = 0 (Data clock idle when LOW), CPHA = 0 (Works with rising edge), SPR1 and SPR0 = 0 (4MHz)

  /* Drivers setup */
  
  // Set direction and step pins
  z_motor.set_step_and_dir_pins(8, 7);
  r_motor.set_step_and_dir_pins(3, 2);

  // Set speed to work with, in SPS (Steps Per Second)
  z_motor.set_speed(200);
  r_motor.set_speed(80);

  // Set diameter
  z_motor.set_diameter(DIAMETER_Z);
  r_motor.set_diameter(DIAMETER_R);

  // Set resolution (THIS IS HARD-WIRED, BUT A RESOLUTION MUST BE SET IN THE CODE TO CORRECTLY CALCULATE THE PULSES NEEDED)
  z_motor.set_resolution(4);
  r_motor.set_resolution(4);

  // Set pins for end-switches, POSSIBLE VALUES: A6, A7 e 9
  z_motor.start_end_switch(A6);
  r_motor.start_end_switch(A7);

  // Start both drivers
  z_motor.start();
  r_motor.start();
  /* Simple delay after start of drivers */
  delay(200);

  /* Initialize both eletromagnets */
  pinMode(Eletro_1, OUTPUT);
  pinMode(Eletro_2, OUTPUT);
}

// SPI interrupt routine
ISR (SPI_STC_vect){
  // Read SPI message sent from BBB
  byte c = SPDR;

  if(waiting_second_byte){
    if(last_byte == 'z'){
      distance_1 = (MAXIMUM_Z/255.0)*((float)c);
    }
    else if(last_byte ==  'r'){
      distance_1 = (MAXIMUM_R/255.0)*((float)c);
    }
    else if(last_byte == 'b'){  // Possible function for moving both motors together
      // Waiting for a third byte
      distance_1 = (MAXIMUM_Z/255.0)*((float)c);
      third_byte = 1;
    }
    else {
      // Error, not expecting another byte now 
      // Send error byte
      RETURN_SPI_FAIL // No need for ';' or return, already in the define
    }
  
    waiting_second_byte = 0;

    RETURN_SPI_SUCCESS  // No need for ';' or return, already in the define
  }
  else if(third_byte){
    if(last_byte == 'b'){
      distance_2 = (MAXIMUM_R/255.0)*((float)c);
    }
    else {
      // Errors
      distance_1 = -1;  // Zero distance_1
      RETURN_SPI_FAIL // No need for ';', already in the define
    }
    third_byte = 0;

    RETURN_SPI_SUCCESS  // No need for ';' or return, already in the define
  }

  switch(c){
    case 'u':
      /* Reads ultrasonic values and returns them */
      last_byte = (byte)'u';
      RETURN_SPI_SUCCESS
      break;  // Safety Break
    case 'p':
      /* Reads color sensor and return reading */
      last_byte = (byte)'p';
      RETURN_SPI_SUCCESS
      break;  // Safety Break
    case 'e':
      /* Turn on Electromagnets, return something? */
      last_byte = (byte)'e';
      turn_on_magnet = 1;
      RETURN_SPI_SUCCESS
      break;  // Safety Break
    case 'd':
      /* Turn off Electromagnets, return something? */
      last_byte = (byte)'d';
      turn_off_magnet = 1;
      RETURN_SPI_SUCCESS
      break;  // Safety Break
    case 'z':
      /* 'Z' axis movement wanted, wait for another reading */
      waiting_second_byte = 1;
      last_byte = (byte)'z';
      RETURN_SPI_SUCCESS
      break;  // Safety Break
    case 'r':
      /* 'R' (radial) axis movement wanted, wait for another reading */
      waiting_second_byte = 1;
      last_byte = (byte)'r';
      RETURN_SPI_SUCCESS
      break;  // Safety Break
    case 'c':
      /* Check if any motor is moving, return how much time  */
      check_movement = 1;
      last_byte = (byte)'c';
      RETURN_SPI_SUCCESS
      break;  // Safety Break
    default:
      // What the fuck do i do here?
      RETURN_SPI_FAIL
      break;  // Safety Break
  }

  if (!active){
    SPDR = 0;
    return;
  }
  
  SPDR = ans[pos];
  if (ans[pos] == 0 || ++pos >= sizeof (ans))
    active = false;
  
}  // end of interrupt service routine (ISR) SPI_STC_vect

void answer_moving(){
  byte maximum_wait = (byte)max(z_motor.moving, r_motor.moving);
  // Send the value to the BBB

  ans[2] = maximum_wait;
  active = true;
  pos = 0;
  SPDR = ans[pos++];  // Write into the SPI buffer
  return; // Safe return 
}

void loop (void){
  if(distance_1>0){
    if(last_byte == 'z'){
      z_motor.move(distance_1, 'a');  // Absolute movement
    }
    else if(last_byte == 'r'){
      r_motor.move(distance_1, 'a');  // Absolute movement
    }
    else if(last_byte == 'b' && distance_2 > 0){
      // Preciso saber como serao lidos dois valores de distancia diferentes
      move_together(z_motor, distance_1, r_motor, distance_2, 'a');
    }
    else {
      // Possible errors
    };
    distance_1 = -1;
    distance_2 = -1;
  }
  else if(check_movement){
    answer_moving();
    check_movement = 0;
  }
  else if(turn_on_magnet){
    if(turn_off_magnet){
      // Erro do caralho, sei nem o que fazer aqui
    }
    else {
      digitalWrite(Eletro_1, HIGH);
      digitalWrite(Eletro_2, HIGH);
    }
    turn_on_magnet = 0;
  }
  else if(turn_off_magnet){
    if(turn_on_magnet){
      // Erro do caralho, sei nem o que fazer aqui
    }
    else {
      digitalWrite(Eletro_1, LOW);
      digitalWrite(Eletro_2, LOW);
    }
    turn_off_magnet = 0;    
  };
}
