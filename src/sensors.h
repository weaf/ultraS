//sensors.h

#ifndef sensors_h
#define sensors_h
#endif

#include <I2C.h>
#include <avr/eeprom.h>
#include "Arduino.h"

// Lidar definition
#define		LIDAR_Adress		0x62		  // LIDAR-lite I2C adress
#define		CommandControll     0x00          // Initiate unit.
#define		InitMeasurement		0x04          // Iinitiate ranging.
#define		ReadDistance2b		0x8f          // Get both High and Low bytes in 1 call

extern uint8_t NACKflag;		// Acknowledge flag.

enum Direction{forward, backward, left, right, Fleft, Fright, Bleft, Bright, stop};
enum Distance{far, uperMid, mid, lowerMid, near, close, tooClose};

// #define N_SAMPLES 10	// number of samples from IR to average

typedef struct S_DISTANCES
{
	int far;
	int mid;
	int uperMid;
	int lowerMid;
	int near;
	int	close;
	int tooClose;
}S_distances;

typedef struct IR_PROXIMITY
{
	uint8_t Pin;		// Left sharp ir-sensor sens pin
	uint8_t sample_counter; // calculates number of recorded samples
	uint8_t calc_flag; // set the flag to do calculation only once
	uint8_t n_samples;	// number of samples from IR to average

	float rawValue;	// the raw value from the sensor
	float avg;	// calculated average of the raw value
	float k;	// 1/Max_Distance factor used to calculate pwm speed corresponding to distace
	float MaxPlusMin; //to calculate Max_distance + Min_Distance
	float temp_avg; // adds the raw values before calculating avg
	float avg_remap; // remapp of the avg value

	enum Distance current_distance;
	struct S_DISTANCES distance;

} Ir_proximity;

typedef struct INIT_IR_EE // struct for eeprom
{
	uint16_t far;
	uint16_t near;
	uint16_t close;
	uint16_t tooClose;
}init_ir_ee;


//Battery status definition
typedef struct BATSTAT
{
	uint8_t analogPin;
	float battAnalogRead;
	float battVoltage;
	float Vconst;

} Batstat;


typedef struct MOTORDRIVER
{
	uint8_t HA0;	//pwm pin
	uint8_t HA1;
	uint8_t HB0;
	uint8_t HB1;

	//	Direction D;
	//	uint8_t speed;
	//	uint8_t turn;

} MotorDriver;


#ifdef __cplusplus
extern "C" {
	#endif

	//***********************************************************************
	//*			Battery														*
	//*																		*
	//***********************************************************************
	void initBatteryVoltage(struct BATSTAT *V, int _analogPin, float Vconst);
	float getBatteryVoltage(struct BATSTAT *V);

	//***********************************************************************
	//*			IR_PROXIMITY												*
	//*																		*
	//***********************************************************************
	void initIRP(struct IR_PROXIMITY *irp, uint8_t irPin, int _far,  int _uperMid, int _mid, int _lowerMid, int _near, int _close, int _tooClose, uint8_t _n_samples);
	void updateIRP(struct IR_PROXIMITY *irp);
	uint8_t irSpeed(struct IR_PROXIMITY *irp);
	void updateIrDistance(struct IR_PROXIMITY *irp);
	//***********************************************************************
	//*			Lidar														*
	//*																		*
	//***********************************************************************
	void initLidar();
	int getLidarDistance();

	//***********************************************************************
	//*			Motors														*
	//*																		*
	//***********************************************************************
	void initMotors(struct MOTORDRIVER *driver, uint8_t A0_Pin, uint8_t A1_Pin, uint8_t B0_Pin, uint8_t B1_Pin);
	void motorControl(struct MOTORDRIVER *driver, uint8_t A0_val, uint8_t A1_val, uint8_t B0_val, uint8_t B1_val);
	void motorDirection(struct MOTORDRIVER *driver, Direction dir, uint8_t speed, uint8_t turn);


	/* Debug functions, Uncomment when used*/
	void Debug1(struct IR_PROXIMITY *irp);

	//***********************************************************************
	//*			calibration														*
	//*																		*
	//***********************************************************************
	void calibrateIR(struct IR_PROXIMITY *irp, struct INIT_IR_EE *ee);

	#ifdef __cplusplus
}
#endif

//Battery status definition end
