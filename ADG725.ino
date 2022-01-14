#include <SPI.h>
#include <avr/io.h>
//#include <TimerOne.h>
// add this to the top of your sketch
#define NOP __asm__ __volatile__ ("nop\n\t")

#define EN  7
#define CSA 6
#define CSB 5





void setup() {
  // Timer1.initialize();
  // Timer1.attachInterrupt(callback);
  // put your setup code here, to run once:
  Serial.begin(9600);
  /* Define directions for port pins */
  DDRB |= _BV(DDB2); // direction output D10
  /* Insert nop for synchronization*/
  NOP; // delay 62.5ns on a 16MHz AtMega
  // begin SPI
  PORTB |= _BV(PB2);
  SPI.beginTransaction(SPISettings(14000000, MSBFIRST, SPI_MODE1));
}

void loop() {
  int temp = getDigits();
  if (temp > 0 && temp <= 16) {
    int SW = temp;
    Serial.println((String) "Switched to channel " + SW);
    SW--;
    uint8_t DATA;
    DATA &= !(_BV(EN) | _BV(CSA) | _BV(CSB));
    DATA |= (SW & 0x0F);
    //Serial.println(DATA, BIN);
    SPI.begin();
    PORTB &= ~_BV(PB2); // connect slave
    SPI.transfer(DATA);
    PORTB |= _BV(PB2);  // disconnect slave
    SPI.end();  // close SPI to prevent high freq noise
  }
  else if (temp != -1)
    Serial.println("Please enter number within 1 to 16");

}


int getDigits() {
  /* return -1 if no digit
      return num if received digits
  */
  static uint8_t power = 0;
  static int temp = -1;
  if (Serial.available() > 0) {
    int num = Serial.read();
    if (!isDigit(num)) {
      num = temp;
      temp = -1;
      power = 0;
      return num;
    }
    else  {
      if (!power) temp++;
      temp *= pow(10, power);
      if (power > 0) power--;
      temp += (num - 48) * pow(10, power++);
    }
  }
  return -1;
}
