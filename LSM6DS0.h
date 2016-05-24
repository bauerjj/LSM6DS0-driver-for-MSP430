/*
 * LSM6DS0.h
 *
 *  Created on: May 17, 2016
 *      Author: justin
 */

#ifndef LSM6DS0_H_
#define LSM6DS0_H_

#include <stdint.h>

extern int8_t LSM6DS0_Init(void);
extern void read(uint8_t* readData, uint8_t addr, uint8_t numBytes);

typedef struct{
	float accel_x;
	float accel_y;
	float accel_z;

	float gyro_x;
	float gyro_y;
	float gyro_z;
}accel_gryo_str;

extern void accelLoad(accel_gryo_str* x_gyro);
extern void gyroLoad(accel_gryo_str* x_gyro);



#endif /* LSM6DS0_H_ */
