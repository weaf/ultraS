//serial.cpp

#include "serial.h"

void initSerial(uint16_t baudrate, uint8_t u2x)
{

// Try u2x mode when higher baudrates are needed to lower error on transmision
  uint16_t baud_setting = (F_CPU / 4 / baudrate - 1) / 2;
  if(u2x)
  {
    UCSR0A |= _BV(U2X0); // UCSR0A |= (1 << U2X0);
  }
  else
  {
    UCSR0A &= ~_BV(U2X0);
  }

  // assign the baud_setting, ubrr (USART Baud Rate Register)
  UBRR0H = baud_setting >> 8;
  UBRR0L = baud_setting;

  UCSR0B |= _BV(RXEN0)| _BV(TXEN0);      // Enable receiver and transmitter
  UCSR0C |= _BV(UCSZ01) | _BV(UCSZ00);    // Set frame: 8data, 1 stp

}

void serial_sendchar( char sendChar)
{
  while ( !( UCSR0A & _BV(UDRE0))); // Wait until transmit buffer is empty
  UDR0 = sendChar; // Put byte on the buffer and send it on TX
}

void serial_sendstring(char *str)
{
	char *tmp = str;
	while(*tmp)
	{
		if(*tmp == '\n')
		{
			serial_sendchar('\r');
		}
		serial_sendchar(*(tmp++));
	}
}

// doesn't work quite good. 
void serial_senfloat(uint8_t *data)
{
	while(!(UCSR0A & _BV(UDRE0)));
	UDR0 = *data;
	while(!(UCSR0A & _BV(UDRE0)));
	UDR0 = *(data+1);
	while(!(UCSR0A & _BV(UDRE0)));
	UDR0 = *(data+2);
	while(!(UCSR0A & _BV(UDRE0)));
	UDR0 = *(data+3);
}

char serial_read( void )
{
    while ( !(UCSR0A & (1<<RXC0)) ); // wait till data is recieved

  return UDR0; // return  the data from the RX buffer

}
