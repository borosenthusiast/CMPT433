#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

enum leds{LED0, LED1, LED2, LED3};
void resetLED();
void initializeLED();
void activateLED(int);
void flashLED_multi(int[], int, long, long, int);