/*Handles all the BBG specific tasks, in this case the potentiometer and the LED display.*/
#ifndef BBG_H
#define BBG_H

#include "bubble.h"
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>


// following the a2d guide's constant values
#define A2D_FILE_VOLTAGE0  "/sys/bus/iio/devices/iio:device0/in_voltage0_raw"
#define A2D_VOLTAGE_REF_V  1.8
#define A2D_MAX_READING    4095
#define PIECEWISE_SEGMENTS 9

//following the i2c guide's constant values
#define I2CDRV_LINUX_BUS0 "/dev/i2c-0"
#define I2CDRV_LINUX_BUS1 "/dev/i2c-1"
#define I2CDRV_LINUX_BUS2 "/dev/i2c-2"

#define I2C_DEVICE_ADDRESS 0x20

#define REG_DIRA 0x00
#define REG_DIRB 0x01
#define REG_OUTA 0x14 //bottom
#define REG_OUTB 0x15 //top

#define LEFT_DIGIT_VALUE "/sys/class/gpio/gpio61/value"
#define LEFT_DIGIT_DIRECTION "/sys/class/gpio/gpio61/direction"

#define RIGHT_DIGIT_VALUE "/sys/class/gpio/gpio44/value"
#define RIGHT_DIGIT_DIRECTION "/sys/class/gpio/gpio44/direction"

#define GPIO_EXPORT "/sys/class/gpio/export"
#define GPIO_LEFT_DIGIT  61
#define GPIO_RIGHT_DIGIT 44

void* BBG_listener(void*);
void BBG_stopListen();
void* i2cThread(void* args);

enum directions{LEFT, RIGHT};

#endif