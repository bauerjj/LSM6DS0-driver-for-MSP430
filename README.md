# LSM6DS0-driver-for-MSP430
Accelerometer and Gyroscope Driver for the LSM6DS0 using the MSP430. Simple API to gather the gyro and accelerometer x,y,z points using SPI.
The GUI Composer tool inside of Code Composer Studio is used to graphically display the data in real-time. For more information, please see
http://mcuhq.com/edit/28/msp430-3-axis-accelerometer-and-gyroscope-example-driver-using-the-lsm6ds0

Sample usage:

    #include "LSM6DS0.h"

    accelLoad(&x_gyro); // get accelerometer results
    gyroLoad(&x_gyro); // get gyro results
    
