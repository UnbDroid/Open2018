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

byte sensorLDR[8]= {'0','1','0','1','0','0','1','0'};
byte rx;
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
  //delayMicroseconds(50);
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
      if (rx == 's') {
        
        rx = SPItransfer('I');
        for (int i = 0; i < 8; i++ ) {
          rx = SPItransfer(sensorLDR[i]);
        }
        rx = SPItransfer(255);
        rx = SPItransfer('\0');
      }
    }
    SSlast = LOW;
}

// The setup() function runs after reset.
void setup() {
  // Initialize SPI Slave.
  SlaveInit();
  Serial.begin(9600);
}

//////////////////////GALERA QUE FOR FAZER A FILTRAGEM INSERE CODIGO AQUI

void filtering()
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
    filtering();
    if (SSlast != HIGH) {
      // Yes, release MISO pin.
      pinMode(MISO, INPUT);
      // Update SSlast.
      SSlast = HIGH;
    }
  }
}
