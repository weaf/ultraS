//motors.cpp

#include "motors.h"


//***********************************************************************
//*			Motor														*
//*																		*
//***********************************************************************
void initMotors(struct MOTORDRIVER *driver, uint8_t A0_Pin, uint8_t A1_Pin, uint8_t B0_Pin, uint8_t B1_Pin)
{
	driver->HA0 = A0_Pin;
	driver->HA1 = A1_Pin;
	driver->HB0 = B0_Pin;
	driver->HB1 = B1_Pin;

	pinMode(A0_Pin,OUTPUT);
	pinMode(A1_Pin,OUTPUT);
	pinMode(B0_Pin,OUTPUT);
	pinMode(B1_Pin,OUTPUT);

}

void motorControl(struct MOTORDRIVER *driver, uint8_t A0_val, uint8_t A1_val, uint8_t B0_val, uint8_t B1_val)
{
	analogWrite(driver->HA0 , A0_val);
	analogWrite(driver->HA1 , A1_val);
	analogWrite(driver->HB0 , B0_val);
	analogWrite(driver->HB1 , B1_val);
}

//void motorDirection(struct MOTORDRIVER *driver, Direction dir)
void motorDirection(struct MOTORDRIVER *driver, Direction dir, uint8_t speed, uint8_t turn)
{
	switch (dir)
	{
		case forward:	motorControl(driver, speed, 0, 0, 0);
		break;
		case backward:	motorControl(driver, 0, speed, 0, 0);
		break;
		case left:		motorControl(driver, 0, 0, turn, 0);
		break;
		case right:		motorControl(driver, 0, 0, 0, turn);
		break;
		case Fleft:		motorControl(driver, speed, 0, turn, 0);
		break;
		case Fright:	motorControl(driver, speed, 0, 0, turn);
		break;
		case Bleft:		motorControl(driver, 0, speed, turn, 0);
		break;
		case Bright:	motorControl(driver, 0, speed, 0, turn);
		break;
		case stop:		motorControl(driver, speed, speed, 0, 0);
		break;
		default:		motorControl(driver, 0, 0, 0, 0);
		break;
	}
}
