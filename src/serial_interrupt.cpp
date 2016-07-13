//serial_interrupt.cpp

#include "serial.h"

// volatile char serial_data;

void initSerial_interrupt(uint16_t baudrate, uint8_t u2x)
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

  UCSR0B |= _BV(RXCIE0);  // Enable recieve interrupt

}


// ISR(USART_RX_vect)
// {
  /* Functions to be implemeted */
//     serial_data = UDR0; // Read data from the RX buffer
//   //  UDR0 = serial_data; // Write data to the TX buffer
// }
