/*Module that uses the alsa api from the completed template and contains the audio related variables*/
#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <inttypes.h>


#include "audiogen.h"

#define BYTES_TO_READ 7


#define I2C1 "/dev/i2c-1"
#define I2C_ADDRESS 0x1C
#define I2C_ACTIVE 0x2A


#define XZTHRESHOLD 18000
#define YDOWNTHRESHOLD 30000
#define YUPTHRESHOLD 3000

void* Accel_Listener(void* args);
void Accel_StopListen();

#endif