// batstat.h is to measure the battery status

#ifndef batstat_h
#define batstat_h

#include "sensor_helper.h"

//Battery status definition
typedef struct BATSTAT
{
	uint8_t analogPin;
	float battAnalogRead;
	float battVoltage;
	float Vconst;

} Batstat;

	//***********************************************************************
	//*			Battery														*
	//*																		*
	//***********************************************************************
	void initBatteryVoltage(struct BATSTAT *V, int _analogPin, float Vconst);
	float getBatteryVoltage(struct BATSTAT *V);


#endif
