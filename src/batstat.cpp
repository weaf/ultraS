//batstat.cpp

#include "batstat.h"

//***********************************************************************
//*			Battery														*
//*																		*
//***********************************************************************
void initBatteryVoltage(struct BATSTAT *V, int _analogPin, float _Vconst)
{
	V->analogPin = _analogPin;
	V->Vconst = _Vconst;
}

float getBatteryVoltage(struct BATSTAT *V)
{
	float temp;
	V->battAnalogRead = analogRead(	V->analogPin);
	temp = 	V->battAnalogRead / V->Vconst;
	V->battVoltage = temp/10; //	int val2 = ((val11 % 100) / 10);
	return V->battVoltage;
}
