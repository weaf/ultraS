#include <Arduino.h>

// Tunnler robot.

/*

Pin configurations
batstat:	A0 				Returns battery status in voltage
irSensors:	A1 A2, A3		Sharp IR proximity sensors
led13:		13 				Onboard led
driver:		5, 6, 10, 11	DC motor pins

Using the 'Arduino I2C Master Library' from DSS Circuits:
http://www.dsscircuits.com/index.php/articles/66-arduino-i2c-master-library


*/
#include "sensors.h"
#include <I2C.h>
#include <Servo.h>
#include "blink.h"


#define		Vconst		4.092	// constant for calculating analog value to voltage

/*
#define		motorA		5	// Rear DC motor
#define		motorB		6	// Rear DC motor
#define		steeringA	10	// Front DC motor
#define		steeringB	11	// Front DC motor
*/

#define	 FLPin	A2	// Left sharp ir-sensor sens pin
#define	 FRPin	A1	// right sharp ir-sensor sens pin
#define	 FMPin	A3	// Front middle sharp ir-sensor sens pin
#define	 startBtn 2

void debug2(const char *direction, const char *Dist1Name, const char *Dist2Name, enum Distance dist1, enum Distance dist2, int speed);

Servo lidarServo;
Blink led13;	// On board led Pin 13
Batstat batstat;
MotorDriver driver;
Ir_proximity ir_FrontLeft;
Ir_proximity ir_FrontRight;
Ir_proximity ir_FrontMiddle;
enum Distance irDistance;

int lidarDistance = 0, pos = 0;
int speed = 0, turn = 0;
uint8_t direction_flag = 1; //change when obsticles in front.

void setup()
{
	Serial.begin(9600);

	initMotors(&driver,5,6,10,11);		//void initMotors(struct MOTORDRIVER *driver, const char *_motor, uint8_t A0_Pin, uint8_t A1_Pin, uint8_t B0_Pin, uint8_t B1_Pin)


	I2c.begin(); 		// Opens & joins the irc bus as master
	delay(100);			// Waits to make sure everything is powered up before sending or receiving data
	I2c.timeOut(50); 	// Sets a timeout to ensure no locking up of sketch if I2C communication fails

	initBlink(&led13, 13, 1000, 500); 	// blink(int _ledPin, long _onDuration, long _offDuration)
	initBatteryVoltage(&batstat, A0, Vconst);

	/***************************************************************************
	* void initIRP(struct IR_PROXIMITY *irp, uint8_t irPin, uint8_t _far,  int _uperMid, int _mid, int _lowerMid, int _near, int _close, int _tooClose)
	* The values is to be entered inverted. for example. If the value for short distance is 600. it is remaped to represent short distance.
	* That is Highest value will represent short distance, Lowest value will represent long distance
	***************************************************************************/

	initIRP(&ir_FrontLeft, FLPin, 660,530,400,450,310,130,40);
	initIRP(&ir_FrontRight, FRPin, 660,530,400,450,310,130,40);
	initIRP(&ir_FrontMiddle, FMPin, 580,530,400,450,310,250,10);

	pinMode(startBtn, INPUT);

	// 8 bit timer. Interrupt somewhere in the middle
	OCR0A = 0xAF;
	TIMSK0 |= _BV(OCIE0A);
}


SIGNAL(TIMER0_COMPA_vect)
{
	unsigned long currentMs = millis();
	updateBlink(&led13, currentMs);
	updateIRP(&ir_FrontMiddle);
	updateIRP(&ir_FrontLeft);
	updateIRP(&ir_FrontRight);
	updateIrDistance(&ir_FrontMiddle);
	updateIrDistance(&ir_FrontLeft);
	updateIrDistance(&ir_FrontRight);
}

void loop()
{

	/*******************************************************************************
	* Calibrated values for ir. left, right: 600 = break 500 = ok
	* front:500 = stop, 200 = hard break,
	*
	* Availible directions
	* forward, backward, left, right,
	* Fleft, Fright, Bleft, Bright
	*
	* enum Direction{forward, backward, left, right, Fleft, Fright, Bleft, Bright};
	*
	* motorDirection(&driver, Direction, speed, turn)
	*******************************************************************************/
	if(digitalRead(startBtn))
	{

		speed = irSpeed(&ir_FrontMiddle);

		// Obsticle close to front and right side
		if(ir_FrontMiddle.current_distance == close  && ir_FrontRight.current_distance == close)
		{
			speed = speed/3;
			motorDirection(&driver, Fleft, speed, speed);
			debug2(" Front_Right "," FM_Dist: ", " FR_Dist: ", ir_FrontMiddle.current_distance, ir_FrontRight.current_distance, speed);

		}

		//Obsitcle close to front and left side
		else if(ir_FrontMiddle.current_distance == close  && ir_FrontLeft.current_distance == close)
		{
			//speed = speed >> 2;
			motorDirection(&driver, Fright, speed, speed);
			debug2(" Front_Left "," FM_Dist: ", " FL_Dist: ", ir_FrontMiddle.current_distance, ir_FrontLeft.current_distance, speed);

		}

		else if(ir_FrontMiddle.current_distance == tooClose)
		{
			motorDirection(&driver, stop, 200, 0); // Stopp immediatly
			motorDirection(&driver, backward, 100, 0);
			delay(100);
			// decide which way to turn
			// which side is closest to the obsticles
			if(ir_FrontLeft.avg_remap < ir_FrontRight.avg_remap)
			{
				while (ir_FrontMiddle.current_distance < near || ir_FrontRight.current_distance < close)
				{
					motorDirection(&driver, Bright, 150, 150);
					debug2(" Backward_Right "," FM_Dist: ", " FL_Dist: ", ir_FrontMiddle.current_distance, ir_FrontLeft.current_distance, speed);
				}
				// motorDirection(&driver, Bright, 150, 150);
				// debug2(" Backward_Right "," FM_Dist: ", " FL_Dist: ", ir_FrontMiddle.current_distance, ir_FrontLeft.current_distance, speed);
				// delay(500);
			}

			else if (ir_FrontLeft.avg_remap > ir_FrontRight.avg_remap)
			{
				motorDirection(&driver, Bleft, 150, 150);
				debug2(" Backward_Right "," FM_Dist: ", " FL_Dist: ", ir_FrontMiddle.current_distance, ir_FrontRight.current_distance, speed);
				delay(1000);
			}
			else
			{
				motorDirection(&driver, Bleft, 150, 150);
				debug2(" stop "," FM_Dist: ", " FL_Dist: ", ir_FrontMiddle.current_distance, ir_FrontLeft.current_distance, speed);
				delay(1000);
			}
		}

		else
		{
			motorDirection(&driver, forward, speed, 0);
			debug2(" Default "," FM_Dist: ", " FL_Dist: ", ir_FrontMiddle.current_distance, ir_FrontLeft.current_distance, speed);
		}
	}

	else
	{
		motorDirection(&driver, stop, 100, 0);
		// Debug1(&ir_FrontMiddle);
	}




	/*
	for(pos = 0; pos <= 180; pos++)
	{
	lidarServo.write(pos);
	lidarDistance = getLidarDistance();
	//	serialPrintRange(pos, distance);
	delay(20);
}
for(pos = 180; pos >= 0; pos--)
{
lidarServo.write(pos);
lidarDistance = getLidarDistance();
//serialPrintRange(pos, distance);
delay(20);
}
*/


}

void debug2(const char *direction, const char *Dist1Name, const char *Dist2Name, enum Distance dist1, enum Distance dist2, int speed)
{
	Serial.print(direction);
	Serial.print(Dist1Name);
	Serial.print(dist1);
	Serial.print(Dist2Name);
	Serial.print(dist2);
	Serial.print(" Speed: ");
	Serial.println(speed);
}
