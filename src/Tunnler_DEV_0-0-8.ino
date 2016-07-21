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
//#include <Servo.h>
#include "blink.h"
#include "motors.h"
#include "lidar.h"
#include "irProx.h"
#include "batstat.h"



#define		Vconst		4.092	// constant for calculating analog value to voltage

/*
#define		motorA		5	// Rear DC motor
#define		motorB		6	// Rear DC motor
#define		steeringA	10	// Front DC motor
#define		steeringB	11	// Front DC motor
*/

#define	 startBtn 2
#define	 DECREAS_SPEED(s,f)	((s)/(f)) // decreases the speed s with factor f

void debug2(const char *direction, const char *Dist1Name, const char *Dist2Name, enum Distance dist1, enum Distance dist2, int speed);
void readIrEE();
void updateIrEE();

//Servo lidarServo;
Blink led13;	// On board led Pin 13
Batstat batstat;
MotorDriver driver;

int pos = 0;
int speed = 0, turn = 0;
uint8_t direction_flag = 1; //change when obsticles in front.
char BTChoise;
uint8_t message = 0;
int prevMS, currMS;

void setup()
{
	Serial.begin(9600);

	initMotors(&driver,5,6,10,11);		//void initMotors(struct MOTORDRIVER *driver, const char *_motor, uint8_t A0_Pin, uint8_t A1_Pin, uint8_t B0_Pin, uint8_t B1_Pin)


	I2c.begin(); 		// Opens & joins the irc bus as master
	delay(100);			// Waits to make sure everything is powered up before sending or receiving data
	I2c.timeOut(50); 	// Sets a timeout to ensure no locking up of sketch if I2C communication fails

	initBlink(&led13, 13, 1000, 500); 	// blink(int _ledPin, long _onDuration, long _offDuration)
	initBatteryVoltage(&batstat, A0, Vconst);

	pinMode(startBtn, INPUT);

	//using Timer0, 8 bit timer, max val 255.
	//compare regiseter val = [ 16,000,000Hz/ (prescaler * desired interrupt frequency) ] - 1
		// OCR0A = (16*10^6)/(64*)
	OCR0A = 0x7F; //Interrupt somewhere in the middle decimal val = 127
	TIMSK0 |= _BV(OCIE0A);
}


ISR(TIMER0_COMPA_vect)
{
	unsigned long currentMs = millis();
	updateBlink(&led13, currentMs);

}

void loop()
{

	/*******************************************************************************
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
	* 4 = Show excicting saved values in eeprom
	**************************/
	// BTChoise = 1;



	if (BTChoise == '1')
	{
		Serial.println("Automatic mode, to exit type 'c' ");
		while(BTChoise != 'c')
		{
			if(digitalRead(startBtn))
			{
				Serial.println("Automatic mode not implemented yet, to exit type 'c' ");
			}

			else
			{
				motorDirection(&driver, stop, 100, 0);
			}

			BTChoise = Serial.read();
			if(BTChoise == 'c')
			{
				motorDirection(&driver, stop, 100, 0);
				message = 0;
				return;
			}

		}

	}

	if (BTChoise == '2')
	{
			Serial.println("       Manual mode      ");
			Serial.println("This mode is not implemeted yet");
			Serial.println("");
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
