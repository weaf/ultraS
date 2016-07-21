//irProx.cpp

#include "irProx.h"



//***********************************************************************
//*			IR_PROXIMITY												*
//*																		*
//***********************************************************************

void initIRP(struct IR_PROXIMITY *irp, uint8_t irPin, int _far, int _near, int _close, int _tooClose, uint8_t _n_samples)
{
	irp->Pin = irPin;		// Pin number the ir is attached to
 	irp->n_samples = _n_samples;
	irp->distance.far = _far;
	irp->distance.near = _near;
	irp->distance.close = _close;
	irp->distance.tooClose = _tooClose;

	irp->k = 1/(float)irp->distance.far; // factor used to calculate pwm speed corresponding to distace
	irp->MaxPlusMin = (irp->distance.far + irp->distance.tooClose);
}

void updateIRP(struct IR_PROXIMITY *irp)
{
	irp->rawValue = analogRead(irp->Pin); 	// read analog value from assigned pin
	irp->temp_avg = irp->temp_avg + irp->rawValue; // add analog value to a temp variable
	irp->sample_counter++; // increas the number of read samples
	if(irp->sample_counter == irp->n_samples)
	{
		irp->sample_counter = 0;
		irp->avg = irp->temp_avg/irp->n_samples; // calculates the avarage
		irp->temp_avg = 0;

		// REmapping:remap = max_val + min_val - current_val
		irp->avg_remap = irp->MaxPlusMin - irp->avg;
	}
}

uint8_t irSpeed(struct IR_PROXIMITY *irp)
{
	float k_avg = 0, speed  = 0;

	k_avg = irp->avg_remap * irp->k; // calculates the scale factor for the speed

	if(k_avg > 1)
	{
		k_avg = 1;
	}
	speed = k_avg * 255; // calculate the PWM value to be used


	return (uint8_t) speed;
}

void updateIrDistance(struct IR_PROXIMITY *irp)
{
	/*
  * Distance check
	* enums 								 far, uperMid, mid, lowerMid, near, close, tooClose
	* remaped sensore values 600,530,400,450,310,130,40
	*/

	if(irp->avg_remap <=  irp->distance.tooClose)
	{
		irp->current_distance = tooClose;
	}
	else if (irp->avg_remap > irp->distance.tooClose && irp->avg_remap <= irp->distance.close )
	{
		irp->current_distance = close;
	}
	else if (irp->avg_remap > irp->distance.close && irp->avg_remap <= irp->distance.near )
	{
		irp->current_distance = near;
	}

	else {irp->current_distance = far;}

}

//***********************************************************************
//*			calibration														*
//*																		*
//***********************************************************************
void calibrateIR(struct IR_PROXIMITY *irp, uint8_t numOfSamples)
{
	char choise = ' ';
		char update[] = "Updating value...";
	char updated[] = "Value updated";
	uint8_t flag = 0;
	irp->n_samples = numOfSamples;
	while(choise != 'c')
	{
		if(!flag)
		{
			Serial.println(" Choose a distanse to calibrate ");
			Serial.println(" 1 = far \n 2 = near \n 3 = close \n 4 = tooClose \n f = finished \n c = Cancel ");
			Serial.println();
			flag = 1;
		}
		while(!Serial.available());
		choise = Serial.read();

		if (choise == '1')
		{
			Serial.println(" poit the sensor to a 'far distance' type 'u' to start calibration ");
			Serial.println();

			while(!Serial.available());
			choise = Serial.read();
			if (choise == 'u')
			{
				Serial.println(update);
				updateIRP(irp);
				irp->distance.far = irp->avg_remap;
				Serial.println(updated);
				flag = 0;
			}
		}
		else if (choise == '2')
		{
			Serial.println(" poit the sensor to a 'near distance' type 'u' to start calibration ");
			Serial.println();

			while(!Serial.available());
			choise = Serial.read();
			if (choise == 'u')
			{
				Serial.println(update);
				updateIRP(irp);
				irp->distance.near = irp->avg_remap;
				Serial.println(updated);
				flag = 0;
			}
		}
		else if (choise == '3')
		{
			Serial.println(" poit the sensor to a 'close distance' type 'u' to start calibration ");
			Serial.println();
			while(!Serial.available());
			choise = Serial.read();
			if (choise == 'u')
			{
				Serial.println(update);
				updateIRP(irp);
				irp->distance.close = irp->avg_remap;
				Serial.println(updated);
				flag = 0;
			}
		}
		else if (choise == '4')
		{
			Serial.println(" poit the sensor to a 'tooClose distance' type 'u' to start calibration ");
			Serial.println();
			while(!Serial.available());
			choise = Serial.read();
			if (choise == 'u')
			{
				Serial.println(update);
				updateIRP(irp);
				irp->distance.tooClose = irp->avg_remap;
				Serial.println(updated);
				flag = 0;
			}
		}
		else if(choise == 'f')
		{
			Serial.println(" Calibration finished, will do som calculations for spped factors... ");
			Serial.println();

			irp->k = 1/(float)irp->distance.far; // factor used to calculate pwm speed corresponding to distace
			irp->MaxPlusMin = (irp->distance.far + irp->distance.tooClose);
			irp->n_samples = 10;

			Serial.println(" Calculations finished ");
			Serial.println(" Sensor are now Calibrated ");
			Serial.println();
			flag = 0;
			return;
		}
		else if(choise == 'c')
		{
			Serial.println(" Canceling calibration");
			irp->n_samples = 10;
			return;
		}
	}

}
