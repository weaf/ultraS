// sensor helper functions and structs to be used for different sensors
#ifndef sensor_helper_h
#define sensor_helper_h

#include "Arduino.h"

enum Direction{forward, backward, left, right, Fleft, Fright, Bleft, Bright, stop};
enum Distance{far, near, close, tooClose};

typedef struct S_DISTANCES
{
	uint16_t far;
	uint16_t near;
	uint16_t close;
	uint16_t tooClose;
}S_distances;


#endif
