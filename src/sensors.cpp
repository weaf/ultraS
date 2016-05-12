//sensors.c

#include "sensors.h"


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



//***********************************************************************
//*			Lidar														*
//*																		*
//***********************************************************************
uint8_t NACKflag;		// Acknowledge flag.

void initLidar()
{
	NACKflag = 1;
	//initiate unit Write 0x04 to register 0x00
	while (NACKflag != 0)
	{
		// Write 0x04 to 0x0, sets NACKflag to 0 when if ACK
		NACKflag = I2c.write(LIDAR_Adress, CommandControll, InitMeasurement);
		delay(1); // prevent overpolling
	}
}

int getLidarDistance()
{
	byte distanceArray[2]; // array to store distance bytes

	initLidar();  //Initiate unit and ranging

	// Read 2byte distance from register 0x8f
	NACKflag = 1;
	while (NACKflag != 0)
	{
		NACKflag = I2c.read(LIDAR_Adress, ReadDistance2b, 2, distanceArray); // Read 2 Bytes from LIDAR-Lite Address and store in array
		delay(1); // prevent overpolling
	}
	int distance = (distanceArray[0] << 8) + distanceArray[1];  // Shift high byte [0] 8 to the left and add low byte [1] to create 16-bit int

	return(distance);
}

//***********************************************************************
//*			IR_PROXIMITY												*
//*																		*
//***********************************************************************

void initIRP(struct IR_PROXIMITY *irp, uint8_t irPin, int _far,  int _uperMid, int _mid, int _lowerMid, int _near, int _close, int _tooClose)
{
	irp->Pin = irPin;		// Pin number the ir is attached to

	irp->distance.far = _far;
	irp->distance.mid = _mid;
	irp->distance.near = _near;
	irp->distance.uperMid = _uperMid;
	irp->distance.lowerMid = _lowerMid;
	irp->distance.close = _close;
	irp->distance.tooClose = _tooClose;
}

void updateIRP(struct IR_PROXIMITY *irp)
{
	irp->rawValue = analogRead(irp->Pin); 	// read analog value from assigned pin
	irp->temp_avg = irp->temp_avg + irp->rawValue; // add analog value to a temp variable
	irp->sample_counter++; // increas the number of read samples
	if(irp->sample_counter == N_SAMPLES)
	{
		irp->sample_counter = 0;
		irp->avg = irp->temp_avg/N_SAMPLES; // calculates the avarage
		irp->temp_avg = 0;

	}
}

uint8_t irSpeed(struct IR_PROXIMITY *irp)
{
	float k_avg = 0, speed  = 0;

	// REmapping:remap = max_val + min_val - current_val
	irp->avg_remap = (irp->distance.far + irp->distance.tooClose) - irp->avg;
	if(!irp->calc_flag)
	{
		irp->k = 1/(float)irp->distance.far; // factor used to calculate pwm speed corresponding to distace
		irp->calc_flag = 1;
	}

	k_avg = irp->avg_remap * irp->k; // calculates the scale factor for teh speed

	speed = k_avg * 255; // calculate the PWM value to be used

	return (uint8_t) speed;
}

void updateIrDistance(struct IR_PROXIMITY *irp)
{
	/*Distance check
	enums 								 far, uperMid, mid, lowerMid, near, close, tooClose
	remaped sensore values 600,530,400,450,310,130,40
	*/

	if(irp->avg_remap <= irp->distance.tooClose)
	{
		irp->current_distance = tooClose;
	}

	else if(irp->avg_remap > irp->distance.tooClose && irp->avg_remap <= irp->distance.close)
	{
		irp->current_distance = close;
	}

	else if(irp->avg_remap > irp->distance.close && irp->avg_remap <= irp->distance.lowerMid)
	{
		irp->current_distance = near;
	}

	else if(irp->avg_remap > irp->distance.lowerMid && irp->avg_remap <= irp->distance.uperMid)
	{
		irp->current_distance = mid;
	}

	else {irp->current_distance = far;}

}



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

//***********************************************************************
//*			MAth functions												*
//*																		*
//***********************************************************************
