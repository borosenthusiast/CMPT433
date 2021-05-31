#include "gpio.h"
#define JSUP "/sys/class/gpio/gpio26/value"
#define JSDOWN "/sys/class/gpio/gpio46/value"
#define JSRIGHT "/sys/class/gpio/gpio47/value"
#define JSLEFT "/sys/class/gpio/gpio65/value"
#define GPIO_EXPORT "/sys/class/gpio/export"

int pollJoyGPIO() {
    long seconds = 0;
    long nanoseconds = 100000000; //poll every 100ms
    struct timespec reqDelay = {seconds, nanoseconds};
    FILE *pFileUp = fopen(JSUP, "r");
    FILE *pFileRight = fopen(JSRIGHT, "r");
    FILE *pFileDown = fopen(JSDOWN, "r");
    FILE *pFileLeft = fopen(JSLEFT, "r");
    if (pFileUp == NULL || pFileRight == NULL || pFileDown == NULL || pFileLeft == NULL) {
        printf("GPIO file incorrect or does not exist.\n");
    }
    const int MAX_LENGTH = 1024;
    char buffUp[MAX_LENGTH];
    char buffRight[MAX_LENGTH];
    char buffDown[MAX_LENGTH];
    char buffLeft[MAX_LENGTH];
    while(true) {
        pFileUp = fopen(JSUP, "r");
        pFileRight = fopen(JSRIGHT, "r");
        pFileDown = fopen(JSDOWN, "r");
        pFileLeft = fopen(JSLEFT, "r");
        fgets(buffUp, MAX_LENGTH, pFileUp);
        fgets(buffRight, MAX_LENGTH, pFileRight);
        fgets(buffDown, MAX_LENGTH, pFileDown);
        fgets(buffLeft, MAX_LENGTH, pFileLeft);
        //printf("test me, %s, %s, %s, %s\n", buffUp, buffRight, buffDown, buffLeft);
        if (strcmp(buffUp, "0") == 0 || atoi(buffUp) == 0) {
            fclose(pFileUp);
            fclose(pFileRight);
            fclose(pFileLeft);
            fclose(pFileDown);
            return UP;
        }
        else if (strcmp(buffRight, "0") == 0 || atoi(buffRight) == 0) {
            fclose(pFileUp);
            fclose(pFileRight);
            fclose(pFileLeft);
            fclose(pFileDown);
            return RIGHT;
        }
        else if (strcmp(buffDown, "0") == 0 || atoi(buffDown) == 0) {
            fclose(pFileUp);
            fclose(pFileRight);
            fclose(pFileLeft);
            fclose(pFileDown);
            return DOWN;
        }
        else if (strcmp(buffLeft, "0") == 0 || atoi(buffLeft) == 0) {
            fclose(pFileUp);
            fclose(pFileRight);
            fclose(pFileLeft);
            fclose(pFileDown);
            return LEFT;
        }
        nanosleep(&reqDelay, (struct timespec *) NULL);
        fclose(pFileUp);
        fclose(pFileRight);
        fclose(pFileLeft);
        fclose(pFileDown);
    }
        fclose(pFileUp);
        fclose(pFileRight);
        fclose(pFileLeft);
        fclose(pFileDown);
    return NONE;
}

void exportGPIO() { //export GPIO pins for joystick
    FILE *pFile = fopen(GPIO_EXPORT, "w");
    long seconds = 0;
    long nanoseconds = 300000000;
    struct timespec reqDelay = {seconds, nanoseconds};

    if (pFile == NULL) {
        printf("ERROR: unable to export file\n");
        exit(-1);
    }

    fprintf(pFile, "%d", 26);
    fclose(pFile);
    nanosleep(&reqDelay, (struct timespec *) NULL);

    pFile = fopen(GPIO_EXPORT, "w");
    if (pFile == NULL) {
        printf("ERROR: unable to export file\n");
        exit(-1);
    }

    fprintf(pFile, "%d", 47);
    fclose(pFile);
    nanosleep(&reqDelay, (struct timespec *) NULL);

    pFile = fopen(GPIO_EXPORT, "w");
    if (pFile == NULL) {
        printf("ERROR: unable to export file\n");
        exit(-1);
    }

    fprintf(pFile, "%d", 46);
    fclose(pFile);
    nanosleep(&reqDelay, (struct timespec *) NULL);

    pFile = fopen(GPIO_EXPORT, "w");
    if (pFile == NULL) {
        printf("ERROR: unable to export file\n");
        exit(-1);
    }

    fprintf(pFile, "%d", 65);
    fclose(pFile);
    nanosleep(&reqDelay, (struct timespec *) NULL);
}