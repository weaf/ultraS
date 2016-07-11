/****
*   Tunnler robot.
*
* Pin configurations
* batstat:	A0 				Returns battery status in voltage
* irSensors:	A1 A2, A3		Sharp IR proximity sensors
* led13:		13 				Onboard led
* driver:		5, 6, 10, 11	DC motor pins

* Using the 'Arduino I2C Master Library' from DSS Circuits:
* http://www.dsscircuits.com/index.php/articles/66-arduino-i2c-master-library
****/

#include <Arduino.h>
#include <I2C.h>
#include <Servo.h>
#include "sensors.h"
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
void readIrEE();
void updateIrEE();

Servo lidarServo;
Blink led13;	// On board led Pin 13
Batstat batstat;
MotorDriver driver;
Ir_proximity ir_FrontLeft;
Ir_proximity ir_FrontRight;
Ir_proximity ir_FrontMiddle;
S_distances EEMEM ee_irFL_val;
S_distances EEMEM ee_irFR_val;
S_distances EEMEM ee_irFM_val;

enum Distance irDistance;

int lidarDistance = 0, pos = 0;
int speed = 0, turn = 0;
uint8_t direction_flag = 1; //change when obsticles in front.
char BTChoise;
uint8_t message = 0;

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



	initIRP(&ir_FrontLeft, FLPin, 660,400,310,40, 10);
	initIRP(&ir_FrontRight, FRPin, 660,400,310,40, 10);
	initIRP(&ir_FrontMiddle, FMPin, 580,400,310,10, 10);

	pinMode(startBtn, INPUT);

	//using Timer0, 8 bit timer, max val 255.
	//compare regiseter val = [ 16,000,000Hz/ (prescaler * desired interrupt frequency) ] - 1
	// OCR0A = (16*10^6)/(64*)
	OCR0A = 0x7F; //Interrupt somewhere in the middle decimal val = 127
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
	if(!message)
	{
		Serial.println("What do you want to do?");
		Serial.println("1: start automatic controll ");
		Serial.println("2: start manual controll");
		Serial.println("3: calibration mode");
		Serial.println("4: view calibrated values");
		Serial.println("");
		message = 1;
	}

	BTChoise = (char)Serial.read();        // read next available byte

	/**************************
	* BTChoise values
	* 1 = start main program
	* 2 = manual controling from external device (to be implemented)
	* 3 = IR_Sharp sensors calibration
	**************************/
	// BTChoise = 1;


	if (BTChoise == '1')
	{
		if(digitalRead(startBtn))
		{

			speed = irSpeed(&ir_FrontMiddle);

			// Obsticle close to front and right side
			if(ir_FrontRight.current_distance == close)
			{
				speed = speed/3;
				motorDirection(&driver, Fleft, speed, speed);
			}

			//Obsitcle close to front and left side
			else if(ir_FrontLeft.current_distance == close)
			{
				//speed = speed >> 2;
				motorDirection(&driver, Fright, speed, speed);
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
					while (ir_FrontMiddle.current_distance < close || ir_FrontRight.current_distance <= close)
					{
						motorDirection(&driver, Bright, 150, 150);
					}
				}

				else if (ir_FrontLeft.avg_remap > ir_FrontRight.avg_remap)
				{
					motorDirection(&driver, Bleft, 150, 150);
					delay(1000);
				}
				else
				{
					motorDirection(&driver, Bleft, 150, 150);
					delay(1000);
				}
			}

			else
			{
				motorDirection(&driver, forward, speed, 0);
			}
		}

		else
		{
			motorDirection(&driver, stop, 100, 0);
			// Debug1(&ir_FrontMiddle);
		}

	}

	if (BTChoise == '2')
	{
			Serial.println("       Manual mode      ");
			Serial.println("This mode is not implemeted yet");
			Serial.println("");
	}

	if (BTChoise == '3')
	{
		message = 0;
		while(BTChoise != 'c')
		{
			if(!message)
			{
				Serial.println("     Calibration mode      \n");
				Serial.println("");
				Serial.println("Type which sensor to calibrate");
				Serial.println(" l = LF \n r = RF \n m = MF \n s = Save \n c = Cancel \n");
				message = 1;
			}

			BTChoise = Serial.read();
			switch (BTChoise) {
				case 'l':	calibrateIR(&ir_FrontLeft, 20); message = 0; break;
				case 'r':	calibrateIR(&ir_FrontRight, 20); message = 0;break;
				case 'm':	calibrateIR(&ir_FrontMiddle, 20); message = 0; break;
			}

			if(BTChoise == 's')
			{
				Serial.println("saving values to eeprom \n ");
				updateIrEE();
				message = 0;
			}
		}
			if(BTChoise == 'c')
			{
				message = 0;
				return;
			}

	}

  if (BTChoise == '4')
	{
		readIrEE();

		Serial.println("FL Values  ");
		Serial.print("far: ");	Serial.print(ir_FrontLeft.distance.far);
		Serial.print(" near: ");	Serial.print(ir_FrontLeft.distance.near);
		Serial.print(" close: ");	Serial.print(ir_FrontLeft.distance.close);
		Serial.print(" tooClose: ");	Serial.println(ir_FrontLeft.distance.tooClose);
		Serial.println();

		Serial.println("FR Values  ");
		Serial.print("far: ");	Serial.print(ir_FrontRight.distance.far);
		Serial.print(" near: ");	Serial.print(ir_FrontRight.distance.near);
		Serial.print(" close: ");	Serial.print(ir_FrontRight.distance.close);
		Serial.print(" tooClose: ");	Serial.println(ir_FrontRight.distance.tooClose);
		Serial.println();

		Serial.println("FM Values  ");
		Serial.print("far: ");	Serial.print(ir_FrontMiddle.distance.far);
		Serial.print(" near: ");	Serial.print(ir_FrontMiddle.distance.near);
		Serial.print(" close: ");	Serial.print(ir_FrontMiddle.distance.close);
		Serial.print(" tooClose: ");	Serial.println(ir_FrontMiddle.distance.tooClose);
		Serial.println();

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
delay(1000);

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

void updateIrEE()
{
	eeprom_update_block(&ir_FrontLeft.distance, &ee_irFL_val, sizeof(INIT_IR_EE));
	eeprom_busy_wait();
	eeprom_update_block(&ir_FrontRight.distance, &ee_irFR_val, sizeof(INIT_IR_EE));
	eeprom_busy_wait();
	eeprom_update_block(&ir_FrontMiddle.distance, &ee_irFM_val, sizeof(INIT_IR_EE));
}

void readIrEE()
{
	eeprom_busy_wait();
	eeprom_read_block(&ir_FrontLeft.distance, &ee_irFL_val, sizeof(INIT_IR_EE));
	eeprom_busy_wait();
	eeprom_read_block(&ir_FrontRight.distance, &ee_irFR_val, sizeof(INIT_IR_EE));
	eeprom_busy_wait();
	eeprom_read_block(&ir_FrontMiddle.distance, &ee_irFM_val, sizeof(INIT_IR_EE));
}
// void updateIrEE()
// {
// 	eeprom_update_block(&ram_irFL_val, &ee_irFL_val, sizeof(INIT_IR_EE));
// 	eeprom_busy_wait();
// 	eeprom_update_block(&ram_irFR_val, &ee_irFR_val, sizeof(INIT_IR_EE));
// 	eeprom_busy_wait();
// 	eeprom_update_block(&ram_irFM_val, &ee_irFM_val, sizeof(INIT_IR_EE));
// }
//
// void readIrEE()
// {
// 	eeprom_busy_wait();
// 	eeprom_read_block(&ram_irFL_val, &ee_irFL_val, sizeof(INIT_IR_EE));
// 	eeprom_busy_wait();
// 	eeprom_read_block(&ram_irFR_val, &ee_irFR_val, sizeof(INIT_IR_EE));
// 	eeprom_busy_wait();
// 	eeprom_read_block(&ram_irFM_val, &ee_irFM_val, sizeof(INIT_IR_EE));
// }
