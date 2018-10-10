/* SPI Slave Demo
 * SPI pin numbers:
 * SCK   13  // Serial Clock.
 * MISO  12  // Master In Slave Out.
 * MOSI  11  // Master Out Slave In.
 * SS    10  // Slave Select
 */
// Include SPI (Serial Peripheral Interface) library. Does not support SPI Slave.
#include <SPI.h>
boolean SSlast = LOW;         // SS last flag.
const byte led = 9;           // Slave LED digital I/O pin.
boolean ledState = HIGH;      // LED state flag.
const byte cmdSensores = 1;        // SPI cmdBtn master command code.
const byte cmdLEDState = 2;   // 
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
  while(!(SPSR & (1<<SPIF)));
  delayMicroseconds(200);
  return SPDR;
}
// The setup() function runs after reset.
void setup() {
  // Initialize serial for troubleshooting.
  Serial.begin(9600);
  // Initialize slave LED pin.
  pinMode(led, OUTPUT);
  digitalWrite(led, ledState);
  // Initialize SPI Slave.
  SlaveInit();
  Serial.println("Slave Initialized");
}
// The loop function runs continuously after setup().
void loop() {
  // Slave Enabled?
  if (!digitalRead(SS)) {
    // Yes, first time?
    if (SSlast != LOW) {
      // Yes, take MISO pin.
      pinMode(MISO, OUTPUT);
      Serial.println("***Slave Enabled.");
      // Write -1 slave response code and receive master command code
      byte rx = SPItransfer('I');
      for(int i = 0; i<11;i++ ){
        rx = SPItransfer('0'+i);
      }
      rx = SPItransfer('\0');
     }
      SSlast = LOW;
    }
//  }
  else {
    // No, first time?
    if (SSlast != HIGH) {
      // Yes, release MISO pin.
      pinMode(MISO, INPUT);
      Serial.println("Slave Disabled.");
      // Update SSlast.
      SSlast = HIGH;
    }
  }
}
