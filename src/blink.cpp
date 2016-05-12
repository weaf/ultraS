//blink.c

#include "blink.h"


void initBlink(struct BLINK *led, uint8_t _ledPin, long _onDuration, long _offDuration)
{
	
	led->ledPin = _ledPin;
	pinMode(_ledPin, OUTPUT);

	led->onDuration = _onDuration;
	led->offDuration = _offDuration;

	led->ledState = LOW;
	led->previousMs = 0;
}

void updateBlink(struct BLINK *led, unsigned long currentMs)
{
	if((led->ledState == LOW) && (currentMs - led->previousMs >= led->offDuration))
	{
		led->ledState = HIGH;	// Led On
		led->previousMs = currentMs;	// Save the current time
		digitalWrite(led->ledPin, led->ledState); 
	}
	else if((led->ledState == HIGH) && (currentMs - led->previousMs >= led->onDuration))
	{
		led->ledState = LOW;		// Led Off
		led->previousMs = currentMs;	// Save the current time
		digitalWrite(led->ledPin, led->ledState);
	}
}

uint8_t getLedState(struct BLINK *led)
{
	return led->ledState;
}
