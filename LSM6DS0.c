/*
 * LSM6DS0.c
 *
 *  Created on: May 17, 2016
 *      Author: justin
 *
 * Inspiration:
 * src: https://developer.mbed.org/teams/ST-Americas-mbed-Team/code/Nucleo_Sensor_Shield/docs/tip/
 * src: https://github.com/ChristopherJD/STM32L053R8/blob/master/Intern_Project/LSM6DS0.c
 * src: https://eewiki.net/pages/viewpage.action?pageId=47644832
 */



#include <msp430.h>
#include <stdint.h>
#include "LSM6DS0.h"
#include "LSM6DS0_Platform.h"


#define CS BIT5 // PORT2, bit5

#define SELECT()  do { P2OUT &= (~CS); __delay_cycles(1000); } while ( 0 )
#define DESELECT()  do { P2OUT |= (CS); __delay_cycles(1000); } while ( 0 )

static uint8_t readByte();
static void setReadMode(uint8_t addr);
static void setWriteMode(uint8_t addr);
static void writeByte(uint8_t data);
static void writeRegister(uint8_t addr, uint8_t data);

/**
 * Initializes the transducer to output gyro and accelerometer data at 238Hz
 */
extern int8_t LSM6DS0_Init(){
	uint8_t deviceId;

	read(&deviceId, LSM6DS0_XG_WHO_AM_I_ADDR, 1); // First detect if module is attached

	if(deviceId != I_AM_LSM6DS0_XG)
		return -1; //error


	writeRegister(LSM6DS0_XG_CTRL_REG1_G, LSM6DS0_G_ODR_238HZ); // enable both gyro and accel at 238 Sampling rate (~4-8mA runtime)
	writeRegister(LSM6DS0_CTRL_REG3_G, HP_EN | 0x00); // enable high-pass filter for gyro
	writeRegister(LSM6DS0_FIFO_CTR, 0x00); // Bypass mode, turn of FIFO

	return 1; // success
}

////////////////////////////////////////////////////////////////
//The LSM6DS0 has all of its registers in 2's complement
////////////////////////////////////////////////////////////////

/**
 * Get all 3-axis accelerometer readings. Convert from mg to g.
 */
extern void accelLoad(accel_gryo_str* x_gyro){
	int16_t tmp;

	read((uint8_t*)&tmp,LSM6DS0_XG_OUT_X_L_XL, 2);
	x_gyro->accel_x = (tmp * 0.061) / 1000.0;
	read((uint8_t*)&tmp,LSM6DS0_XG_OUT_Y_L_XL, 2);
	x_gyro->accel_y = (tmp * 0.061) / 1000.0;
	read((uint8_t*)&tmp,LSM6DS0_XG_OUT_Z_L_XL, 2);
	x_gyro->accel_z = (tmp * 0.061) / 1000.0;
}

/**
 * Get all 3-axis gyro readings. Convert from mdps to dps.
 */
extern void gyroLoad(accel_gryo_str* x_gyro){
	int16_t tmp;;

	read((uint8_t*)&tmp,LSM6DS0_XG_OUT_X_L_G, 2);
	x_gyro->gyro_x = (tmp * 8.75) / 1000.0;
	read((uint8_t*)&tmp,LSM6DS0_XG_OUT_Y_L_G, 2);
	x_gyro->gyro_y = (tmp * 8.75) / 1000.0;
	read((uint8_t*)&tmp,LSM6DS0_XG_OUT_Z_L_G, 2);
	x_gyro->gyro_z = (tmp * 8.75) / 1000.0;
}

/**
 * Reads any number of bytes from a specific address. Make sure that
 * the sensor will self-increment its register pointer after subsequent reads
 */
extern void read(uint8_t* readData, uint8_t addr, uint8_t numBytes){
	uint8_t i;
	uint8_t tmp;
	SELECT();
	__delay_cycles(1000);
	setReadMode(addr);
	for(i = 0; i != numBytes; i++){
		*readData = readByte();
		readData++; // increase to next address
	}
	__delay_cycles(1000);
	DESELECT();
}

/**
 * Writes a single byte to a single register
 */
static void writeRegister(uint8_t addr, uint8_t data){
	SELECT();
	__delay_cycles(1000);
	setWriteMode(addr);
	writeByte(data);
	__delay_cycles(1000);
	DESELECT();
}

/**
 * Write an initial byte
 */
static void setWriteMode(uint8_t addr){
	__delay_cycles(1000);
	while (!(IFG2 & UCB0TXIFG)); // TX buffer ready?
	UCB0TXBUF = (addr | 0x00); // 1 = read / 0 = write
	while (!(IFG2 & UCB0TXIFG)); //  TX buffer ready?
	__delay_cycles(1000);
}

/**
 * Write a single byte
 */
static void writeByte(uint8_t data){
	__delay_cycles(1000);
	while (!(IFG2 & UCB0TXIFG)); // TX buffer ready?
	UCB0TXBUF = data; // send data
	while (!(IFG2 & UCB0TXIFG)); //  TX buffer ready?
	__delay_cycles(1000);
}

/**
 * Read a single byte
 */
static void setReadMode(uint8_t addr){
	__delay_cycles(1000);
	while (!(IFG2 & UCB0TXIFG)); // TX buffer ready?
	UCB0TXBUF = (addr | 0x80); // 1 = read / 0 = write
	while (!(IFG2 & UCB0TXIFG)); //  TX buffer ready?
	__delay_cycles(1000);
}


static uint8_t readByte(){
	__delay_cycles(1000);
	while (!(IFG2 & UCB0TXIFG)); // TX buffer ready?
	UCB0TXBUF = 0xAA; // send garbage
	while (!(IFG2 & UCB0TXIFG)); //  TX buffer ready?
	while (!(IFG2 & UCB0RXIFG)); //  RX Received?
	__delay_cycles(1000);

	return UCB0RXBUF; // Store received data
}
