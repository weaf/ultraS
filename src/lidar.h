//lidar.h

#ifndef lidar_h
#define lidar_h

#include <I2C.h>

// Lidar definition
#define		LIDAR_Adress		0x62		  // LIDAR-lite I2C adress
#define		CommandControll 0x00          // Initiate unit.
#define		InitMeasurement	0x04          // Iinitiate ranging.
#define		ReadDistance2b	0x8f

extern uint8_t NACKflag;		// Acknowledge flag.

	//***********************************************************************
	//*			Lidar														*
	//*																		*
	//***********************************************************************
	void initLidar();
	int getLidarDistance();

  #endif
