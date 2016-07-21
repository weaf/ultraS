// lidar.cpp

#include "lidar.h"

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
