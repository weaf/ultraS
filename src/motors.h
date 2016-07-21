//motors.h

#include "sensor_helper.h"

typedef struct MOTORDRIVER
{
	uint8_t HA0;	//pwm pin
	uint8_t HA1;
	uint8_t HB0;
	uint8_t HB1;

} MotorDriver;


//***********************************************************************
//*			Motors														*
//*																		*
//***********************************************************************
void initMotors(struct MOTORDRIVER *driver, uint8_t A0_Pin, uint8_t A1_Pin, uint8_t B0_Pin, uint8_t B1_Pin);
void motorControl(struct MOTORDRIVER *driver, uint8_t A0_val, uint8_t A1_val, uint8_t B0_val, uint8_t B1_val);
void motorDirection(struct MOTORDRIVER *driver, Direction dir, uint8_t speed, uint8_t turn);
