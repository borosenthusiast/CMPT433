#include "led.h"

#define DA_TRIGGER_LED0 "/sys/class/leds/beaglebone:green:usr0/trigger"
#define DA_TRIGGER_LED1 "/sys/class/leds/beaglebone:green:usr1/trigger"
#define DA_TRIGGER_LED2 "/sys/class/leds/beaglebone:green:usr2/trigger"
#define DA_TRIGGER_LED3 "/sys/class/leds/beaglebone:green:usr3/trigger"
#define DA_BRIGHTNESS_LED0 "/sys/class/leds/beaglebone:green:usr0/brightness"
#define DA_BRIGHTNESS_LED1 "/sys/class/leds/beaglebone:green:usr1/brightness"
#define DA_BRIGHTNESS_LED2 "/sys/class/leds/beaglebone:green:usr2/brightness"
#define DA_BRIGHTNESS_LED3 "/sys/class/leds/beaglebone:green:usr3/brightness"


void resetLED() {
    FILE *pLedTriggerFileLED0 = fopen(DA_TRIGGER_LED0, "w");

    if (pLedTriggerFileLED0 != NULL) {
        fprintf(pLedTriggerFileLED0, "heartbeat");
        fclose(pLedTriggerFileLED0);
    }
    else {
        printf("Error opening the LED trigger files.");
    }

    FILE *pLedTriggerFileLED1 = fopen(DA_TRIGGER_LED1, "w");

    if (pLedTriggerFileLED1 != NULL) {
        fprintf(pLedTriggerFileLED1, "mmc0");
        fclose(pLedTriggerFileLED1);
    }
    else {
        printf("Error opening the LED trigger files.");
    }
    
    FILE *pLedTriggerFileLED2 = fopen(DA_TRIGGER_LED2, "w");

    if (pLedTriggerFileLED2 != NULL) {
        fprintf(pLedTriggerFileLED2, "cpu0");
        fclose(pLedTriggerFileLED2);
    }
    else {
        printf("Error opening the LED trigger files.");
    }
    FILE *pLedTriggerFileLED3 = fopen(DA_TRIGGER_LED3, "w");
    if (pLedTriggerFileLED3 != NULL) {
        fprintf(pLedTriggerFileLED3, "mmc1");
        fclose(pLedTriggerFileLED3);
    }
    else {
        printf("Error opening the LED trigger files.");
    }
}

void initializeLED() {
    FILE *pLedTriggerFile = fopen(DA_TRIGGER_LED0, "w");

    if (pLedTriggerFile != NULL) {
        fprintf(pLedTriggerFile, "none"); // set to manual control
    }
    else {
        printf("Error in opening trigger LED0.");
        exit(-1);
    }
    fclose(pLedTriggerFile);

    FILE *pLedTriggerFile1 = fopen(DA_TRIGGER_LED1, "w");
    if (pLedTriggerFile1 != NULL) {
        fprintf(pLedTriggerFile1, "none"); // set to manual control
    }
    else {
        printf("Error in opening trigger LED1.");
        exit(-1);
    }
    fclose(pLedTriggerFile1);

    FILE *pLedTriggerFile2 = fopen(DA_TRIGGER_LED2, "w");
    if (pLedTriggerFile2 != NULL) {
        fprintf(pLedTriggerFile2, "none"); // set to manual control
    }
    else {
        printf("Error in opening trigger LED2.");
        exit(-1);
    }
    fclose(pLedTriggerFile2);

    FILE *pLedTriggerFile3 = fopen(DA_TRIGGER_LED3, "w");
    if (pLedTriggerFile3 != NULL) {
        fprintf(pLedTriggerFile3, "none"); // set to manual control
    }
    else {
        printf("Error in opening trigger LED3.");
        exit(-1);
    }
    fclose(pLedTriggerFile3);
}

void flashLED_multi(int ids[], int idsLen, long durationOnNs, long durationOffNs, int iterations) {
    FILE *pLedBrightnessFileLED0 = fopen(DA_BRIGHTNESS_LED0, "w");
    FILE *pLedBrightnessFileLED1 = fopen(DA_BRIGHTNESS_LED1, "w");
    FILE *pLedBrightnessFileLED2 = fopen(DA_BRIGHTNESS_LED2, "w");
    FILE *pLedBrightnessFileLED3 = fopen(DA_BRIGHTNESS_LED3, "w");

    //printf("test\n");
    if (pLedBrightnessFileLED0 != NULL && pLedBrightnessFileLED3 != NULL && pLedBrightnessFileLED1 != NULL && pLedBrightnessFileLED2 != NULL) {
        for (int i = 0; i < idsLen; i++) {
            switch (ids[i]) {
                case LED0:
                    fprintf(pLedBrightnessFileLED0, "1");
                    break;
                case LED1:
                    fprintf(pLedBrightnessFileLED1, "1");
                    break;
                case LED2:
                    fprintf(pLedBrightnessFileLED2, "1");
                    break;
                case LED3:
                    fprintf(pLedBrightnessFileLED3, "1");
                    break;
            }
        }
        struct timespec reqDelay = {0, durationOnNs};
        nanosleep(&reqDelay, (struct timespec *) NULL);

        fclose(pLedBrightnessFileLED0);
        fclose(pLedBrightnessFileLED1);
        fclose(pLedBrightnessFileLED2);
        fclose(pLedBrightnessFileLED3);

        pLedBrightnessFileLED0 = fopen(DA_BRIGHTNESS_LED0, "w");
        pLedBrightnessFileLED1 = fopen(DA_BRIGHTNESS_LED1, "w");
        pLedBrightnessFileLED2 = fopen(DA_BRIGHTNESS_LED2, "w");
        pLedBrightnessFileLED3 = fopen(DA_BRIGHTNESS_LED3, "w");
        
        // after sleep turn them off.
        fprintf(pLedBrightnessFileLED0, "0");
        fprintf(pLedBrightnessFileLED1, "0");
        fprintf(pLedBrightnessFileLED2, "0");
        fprintf(pLedBrightnessFileLED3, "0");

        // then close the files
        fclose(pLedBrightnessFileLED0);
        fclose(pLedBrightnessFileLED1);
        fclose(pLedBrightnessFileLED2);
        fclose(pLedBrightnessFileLED3);

        //support for multiple time flashing
        if (iterations > 1) {
            struct timespec reqDelayOff = {0, durationOffNs};
            nanosleep(&reqDelayOff, (struct timespec *) NULL);
            flashLED_multi(ids, idsLen, durationOnNs, durationOffNs, iterations - 1);
        }
    }
}

void activateLED(int id) { // ID to turn ON which will turn the other OFF
    FILE *pLedBrightnessFileLED0 = fopen(DA_BRIGHTNESS_LED0, "w");
    FILE *pLedBrightnessFileLED3 = fopen(DA_BRIGHTNESS_LED3, "w");

    if (pLedBrightnessFileLED0 != NULL && pLedBrightnessFileLED3 != NULL) {
        if (id == 0) {
            fprintf(pLedBrightnessFileLED0, "1");
            fprintf(pLedBrightnessFileLED3, "0");
        }
        else if (id == 3) {
            fprintf(pLedBrightnessFileLED0, "0");
            fprintf(pLedBrightnessFileLED3, "1");
        }
        else {
            printf("Incorrect ID.");
        }
    }
    fclose(pLedBrightnessFileLED3);
    fclose(pLedBrightnessFileLED0);
}