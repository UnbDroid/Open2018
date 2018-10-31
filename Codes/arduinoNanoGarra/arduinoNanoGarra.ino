//o vetor de comunicacao comeca com o byte I e termina com o byte F
volatile byte dados [17] = "II123113121121FF";

volatile int pos;
volatile bool active;
void setup (void)
{
  // have to send on master in, *slave out*
  pinMode(MISO, OUTPUT);
  // turn on SPI in slave mode
  SPCR |= bit(SPE);
  
  // turn on interrupts
  SPCR |= bit(SPIE);
}  // end of setup


// SPI interrupt routine
ISR (SPI_STC_vect)
{
  byte c = SPDR;
  if (c == 'g')
  {
    active = true;
    pos = 0;
    SPDR = dados [pos++];   // send first byte
    //while (!(SPSR & (1 << SPIF)));
    return;
  }

  if (!active)
  {
    SPDR = 0;

    return;
  }

  SPDR = sensors [pos];
  if (sensors [pos] == 0 || ++pos >= sizeof (sensors))
    active = false;
}  // end of interrupt service routine (ISR) SPI_STC_vect

void loop (void)
{
  
}
