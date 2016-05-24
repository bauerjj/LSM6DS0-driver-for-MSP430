/**
 *
 * - GUI Composer creates nice graph to display all sensor output readings and displacement in real-time (200ms update)
 * - Every 1second, red LED toggles
 *
 * @author Justin Bauer <mcuhq.com>
 * @date June 2016
 */

/*
 * ======== Standard MSP430 includes ========
 */
#include <msp430.h>

/*
 * ======== Grace related includes ========
 */
#include <ti/mcu/msp430/Grace.h>


#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "LSM6DS0.h"
#include "LSM6DS0_Platform.h"

//////////////////////////////////////////
// Globals to be printed to GUI Composer (must be global!!)


accel_gryo_str x_gyro;	// main gyro struct that contains all sensor outputs

/*
 * Integrate the gyro Z-axis in order to get displacement. This gets
 * full of error if left integrating too long. Future code should
 * clear it after a timeout has occured. For now this will d
 */
float angle = 0;

/*
 * In order to find the actual sample rate, increment this variable and then place a
 * debug point in the 1 second ISR. Look at how many counts occured in the main loop
 * and then use this to number to integrate the gyro sensor input. The end result is
 * an approximate displacement angle from velocity.
 */
uint32_t counts;
#define SAMPLE_RATE	111 // experimentally determined


int main(void)
{

	float offset;	// record the gyro offset on startup in order to minimize integration error over-time
    Grace_init(); // Activate Grace-generated configuration
    LSM6DS0_Init(); // initialize sensor
    gyroLoad(&x_gyro);			// get the gyro readings
    offset = x_gyro.gyro_z;		// Set the original offset

    // Constantly read sensors and wait for 2 full rotations before turning on an LED
    // If accelerometer is moved in any axis to +1.7G and then to -1.7G, toggle another LED
    while(1){
    	accelLoad(&x_gyro); // get accelerometer results
    	gyroLoad(&x_gyro); // get gyro results
    	angle += (x_gyro.gyro_z - offset) / SAMPLE_RATE; // get angle displacement
    	//counts++;		// testing!
    }
}



