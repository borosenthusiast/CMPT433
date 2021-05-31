#ifndef GPIO_H
#define GPIO_H
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>


enum direction{NONE, UP, RIGHT, DOWN, LEFT, PUSH};
int pollJoyGPIO();
void exportGPIO();

#endif