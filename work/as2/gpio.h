#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>


enum direction{NONE, UP, RIGHT, DOWN, LEFT};
int pollJoyGPIO();
void exportGPIO();