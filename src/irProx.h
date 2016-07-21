


#ifndef irProx_h
#define irProx_h

#include "sensor_helper.h"


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

#ifdef __cplusplus
extern "C" {
	#endif

	//***********************************************************************
	//*			IR_PROXIMITY												*
	//*																		*
	//***********************************************************************
	void initIRP(struct IR_PROXIMITY *irp, uint8_t irPin, int _far, int _near, int _close, int _tooClose, uint8_t _n_samples);
	void updateIRP(struct IR_PROXIMITY *irp);
	uint8_t irSpeed(struct IR_PROXIMITY *irp);
	void updateIrDistance(struct IR_PROXIMITY *irp);
	void calibrateIR(struct IR_PROXIMITY *irp, uint8_t numOfSamples);

#ifdef __cplusplus
}
#endif
#endif
