// blink.h

#ifndef blink_h
#define blink_h


#include "Arduino.h"


typedef struct BLINK
{
	uint8_t ledPin;			// Pin number the led is attached to
	long onDuration;	// Time in millseconds that the led is on
	long offDuration;	// Time in millseconds that the led is off

	int ledState;		// Used to change the state of he led
	unsigned long previousMs;	// Saves last time ledstate was updated

} Blink;

#ifdef __cplusplus
extern "C" {
#endif

void initBlink(struct BLINK *led, uint8_t _ledPin, long _onDuration, long _offDuration);
void updateBlink(struct BLINK *led, unsigned long currentMs);
uint8_t getLedState(struct BLINK *led);

#ifdef __cplusplus
}
#endif

#endif