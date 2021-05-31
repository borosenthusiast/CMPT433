#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "led.h"
#include "gpio.h"

int main() {
    srand(time(NULL));
    exportGPIO();
    const int LED_DOWN = 3;
    const int LED_UP = 0;
    initializeLED();
    printf("hello embedded world from orion hsu!\n");
    bool target;
    int score = 0;
    int option = NONE;
    long seconds = 1;
    long nanoseconds = 0;
    struct timespec reqDelay = {seconds, nanoseconds};
    for (int i = 0; i < 8; i++) {
        target = rand() & 1;
        printf("Press the Zen cape's Joystick in the direction of the LED.\n");
        printf("\t UP for LED 0 (top)\n");
        printf("\t DOWN for LED 3 (bottom)\n");
        printf("Press joystick. Current score (%d / 8)\n", score);
        if (target) {
            activateLED(LED_UP);
        }
        else {
            activateLED(LED_DOWN);
        }
        nanosleep(&reqDelay, (struct timespec *) NULL);
        while (option == NONE) {
            option = pollJoyGPIO();
            printf(target ? "true\n" : "false\noption = %d\n", option);
            if (target == true && option == UP){
                printf("Correct! +1 to score \n");
                score += 1;
                int leds[] = {LED0, LED1, LED2, LED3};
                flashLED_multi(leds, 4, 100000000, 0, 1);
            }
            else if (target == false && option == DOWN) {
                printf("Correct! +1 to score \n");
                score += 1;
                int leds[] = {LED0, LED1, LED2, LED3};
                flashLED_multi(leds, 4, 100000000, 0, 1);
            }
            else if (option == RIGHT || option == LEFT) {
                printf("You have selected to exit program.\nScore is %d\n", score);
                resetLED();
                exit(1);
            }
            else {
                printf("Incorrect Response.\n");
                int leds[] = {LED0, LED1, LED2, LED3};
                flashLED_multi(leds, 4, 100000000, 100000000, 5);
            }
        }
        option = NONE;

        nanosleep(&reqDelay, (struct timespec *) NULL);
    }
    printf("You got a score of %d out of 8!\nThanks for playing!\n", score);
    resetLED();
    return 0;
}