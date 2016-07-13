// serial.h
#ifndef serial_h
#define serial_h

#include <avr/io.h>

#ifdef __cplusplus
extern "C" {
	#endif

void initSerial(uint16_t baudrate, uint8_t u2x);
// void initSerial_interrupt(uint16_t baudrate, uint8_t u2x);
void serial_sendchar( char sendChar);
void serial_sendstring(char *str);
void serial_senfloat(uint8_t *data);
char serial_read( void );

#ifdef __cplusplus
}
#endif



#endif
