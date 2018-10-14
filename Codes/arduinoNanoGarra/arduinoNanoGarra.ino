/* SPI Slave Demo
   SPI pin numbers:
   SCK   13  // Serial Clock.
   MISO  12  // Master In Slave Out.
   MOSI  11  // Master Out Slave In.
   SS    10  // Slave Select
*/
// Include SPI (Serial Peripheral Interface) library. Does not support SPI Slave.
#include <SPI.h>

boolean SSlast = LOW;         // SS last flag.
const byte cmdSensores = 's';        // SPI cmdBtn master command code.

uint8_t pos_x, pos_y;

//Initialize SPI slave.

void SlaveInit(void) {
  // Initialize SPI pins.
  pinMode(SCK, INPUT);
  pinMode(MOSI, INPUT);
  pinMode(MISO, INPUT);
  pinMode(SS, INPUT);
  // Enable SPI as slave.
  SPCR = (1 << SPE);
}

// SPI Transfer.
byte SPItransfer(byte value) {
  SPDR = value;
  while (!(SPSR & (1 << SPIF)));
  delayMicroseconds(5);
  return SPDR;
}

void treatSPI()
{
  // Yes, first time?
    if (SSlast != LOW) {
      // Yes, take MISO pin.
      pinMode(MISO, OUTPUT);
      // Write -1 slave response code and receive master command code
      rx = SPItransfer(255);
      rx = SPItransfer(255);
      if (rx = 'x') {
        //byte rx = SPItransfer('I');
        pos_x = SPItransfer('o');
      }else if (rx = 'y') {
        //byte rx = SPItransfer('I');
        pos_y = SPItransfer('o');
      }
    }
    rx = SPItransfer(255);
    rx = SPItransfer(255);
    SSlast = LOW;
}

// The setup() function runs after reset.
void setup() {
  // Initialize SPI Slave.
  SlaveInit();
  Serial.begin(9600);
}

//////////////////////coloque execucao do movimento aqui

void executaMov()
{
}



// The loop function runs continuously after setup().
void loop() {
  // Slave Enabled?
  if (!digitalRead(SS)) {
      treatSPI();

  }
  else {
    // No, first time?
    executaMov();
    if (SSlast != HIGH) {
      // Yes, release MISO pin.
      pinMode(MISO, INPUT);
      // Update SSlast.
      SSlast = HIGH;
    }
    Serial.println("pos_x="+ String(pos_x) + "pos_y="+ String(pos_y));
  }
}
