#include "bbg.h"


bool bbgStatus = true;
long long lastSortedValue = 0;
pthread_mutex_t potMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t i2cMutex = PTHREAD_MUTEX_INITIALIZER;

const double PIECEWISE_Y_DELTA[] =  {19, 40, 60, 130, 50, 200, 300, 400, 900};
const double PIECEWISE_X_DELTA[] =  {500, 500, 500, 500, 500, 500, 500, 500, 100};
const int PIECEWISE_CONSTANT[] = {-1, 20, 60, 270, -50, 700, 1300, 2000, 34800};
const int PIECEWISE_BREAKS[] =   {0, 500, 1000, 1500, 2000, 2500, 3000, 3500, 4000, 4100};

static long long calcSortRate();
static int piecewiseConverter(int a2dValue);
static int a2dToArraySize();

static void initI2C();
static int initI2cBus(char* bus, int address);
static void writeI2cReg(int i2cFileDesc, unsigned char regAddr, unsigned char value);
static void updateSegments();
static void displayDigits(int direction);
static void exportGPIO();

struct displaySegment {
    int left;
    int right;
};

struct displaySegment currentDisplayedSegment = {0, 0};

static long long calcSortRate() {
    //pthread_mutex_lock(&i2cMutex);
    long long lsv = lastSortedValue;
    long long currentNumSorted = Sorter_getNumSorted();
    if (currentNumSorted - lsv == 1) { // if a single array has been sorted singe the last second
        lastSortedValue = currentNumSorted;
        return 1;
    }
    long long sortingRate = (currentNumSorted - lsv); // No need to divide by 60, since the calculation occurs every one second.
    lastSortedValue = currentNumSorted;
    //pthread_mutex_unlock(&i2cMutex);
    return sortingRate;
}


static int piecewiseConverter(int a2dValue) {
    //piecewise functions defined as (change in Y / change in X) * a2dValue - constant to get endpoint values
    // first check and clip values if necessary.
    if (a2dValue < PIECEWISE_BREAKS[0]) {
        a2dValue = 0;
    }
    else if (a2dValue > PIECEWISE_BREAKS[9]) {
        a2dValue = 4100;
    }
    //printf("a2dv %d\n", a2dValue);
    if (a2dValue >= PIECEWISE_BREAKS[0] && a2dValue <= PIECEWISE_BREAKS[1]) {
        float slope = PIECEWISE_Y_DELTA[0] / PIECEWISE_X_DELTA[0]; // Y over X
        return (int)(a2dValue * slope - PIECEWISE_CONSTANT[0]);
    }
    else if (a2dValue > PIECEWISE_BREAKS[1] && a2dValue <= PIECEWISE_BREAKS[2]) {
        float slope = PIECEWISE_Y_DELTA[1] / PIECEWISE_X_DELTA[1];
        return (int)(a2dValue * slope - PIECEWISE_CONSTANT[1]);
    }
    else if (a2dValue > PIECEWISE_BREAKS[2] && a2dValue <= PIECEWISE_BREAKS[3]) {
        float slope = PIECEWISE_Y_DELTA[2] / PIECEWISE_X_DELTA[2];
        return (int)(a2dValue * slope - PIECEWISE_CONSTANT[2]);
    }
    else if (a2dValue > PIECEWISE_BREAKS[3] && a2dValue <= PIECEWISE_BREAKS[4]) {
        float slope = PIECEWISE_Y_DELTA[3] / PIECEWISE_X_DELTA[3];
        return (int)(a2dValue * slope - PIECEWISE_CONSTANT[3]);
    }
    else if (a2dValue > PIECEWISE_BREAKS[4] && a2dValue <= PIECEWISE_BREAKS[5]) {
        float slope = PIECEWISE_Y_DELTA[4] / PIECEWISE_X_DELTA[4];
        return (int)(a2dValue * slope - PIECEWISE_CONSTANT[4]);
    }
    else if (a2dValue > PIECEWISE_BREAKS[5] && a2dValue <= PIECEWISE_BREAKS[6]) {
        float slope = PIECEWISE_Y_DELTA[5] / PIECEWISE_X_DELTA[5];
        return (int)(a2dValue * slope - PIECEWISE_CONSTANT[5]);
    }
    else if (a2dValue > PIECEWISE_BREAKS[6] && a2dValue <= PIECEWISE_BREAKS[7]) {
        float slope = PIECEWISE_Y_DELTA[6] / PIECEWISE_X_DELTA[6];
        return (int)(a2dValue * slope - PIECEWISE_CONSTANT[6]);
    }
    else if (a2dValue > PIECEWISE_BREAKS[7] && a2dValue <= PIECEWISE_BREAKS[8]) {
        float slope = PIECEWISE_Y_DELTA[7] / PIECEWISE_X_DELTA[7];
        return (int)(a2dValue * slope - PIECEWISE_CONSTANT[7]);
    }
    else if (a2dValue > PIECEWISE_BREAKS[8] && a2dValue <= PIECEWISE_BREAKS[9]) {
        float slope = PIECEWISE_Y_DELTA[8] / PIECEWISE_X_DELTA[8];
        return (int)(a2dValue * slope - PIECEWISE_CONSTANT[8]);
    }
    else {
        return 1;
    }
}

static int a2dToArraySize() {
    FILE* a2d = fopen(A2D_FILE_VOLTAGE0, "r");
    if (!a2d) {
        printf("ERROR: Unable to open voltage input file. Cape loaded?\n");
        fclose(a2d);
        return 1;
    }
    int a2dReading = 0;
    int itemsRead = fscanf(a2d, "%d", &a2dReading);
    if (itemsRead <= 0) {
        printf("ERROR: Unable to read values from voltage input file.\n");
        fclose(a2d);
        return 1;
    }

    fclose(a2d);

    return piecewiseConverter(a2dReading);

}

static void exportGPIO() { //export GPIO pins for LEDs (based off exportGPIO from as1)
    FILE *pFile = fopen(GPIO_EXPORT, "w");
    long seconds = 0;
    long nanoseconds = 300000000;
    struct timespec reqDelay = {seconds, nanoseconds};

    if (pFile == NULL) {
        printf("ERROR: unable to export file. Check system config.\n");
        fclose(pFile);
        pthread_exit(NULL);
    }

    fprintf(pFile, "%d", GPIO_LEFT_DIGIT);
    fclose(pFile);
    nanosleep(&reqDelay, (struct timespec *) NULL);

    pFile = fopen(GPIO_EXPORT, "w");
    if (pFile == NULL) {
        printf("ERROR: unable to export file. Check system config\n");
        fclose(pFile);
        pthread_exit(NULL);
    }

    fprintf(pFile, "%d", GPIO_RIGHT_DIGIT);
    fclose(pFile);
    nanosleep(&reqDelay, (struct timespec *) NULL);

    pFile = fopen(LEFT_DIGIT_DIRECTION, "w");
    if (pFile == NULL) {
        printf("ERROR: unable to export file. Check system config.\n");
        fclose(pFile);
        pthread_exit(NULL);
    }

    fprintf(pFile, "out");
    fclose(pFile);
    nanosleep(&reqDelay, (struct timespec *) NULL);
    
    pFile = fopen(RIGHT_DIGIT_DIRECTION, "w");
    if (pFile == NULL) {
        printf("ERROR: unable to export file. Check system config.\n");
        fclose(pFile);
        pthread_exit(NULL);
    }

    fprintf(pFile, "out");
    fclose(pFile);
    nanosleep(&reqDelay, (struct timespec *) NULL);
}

static void initI2C() {
    system("config-pin P9_18 i2c");
    system("config-pin P9_17 i2c");
    exportGPIO();
}

void* BBG_listener(void* args) {
    int currentSize = -1;
    while (bbgStatus) {
        currentSize = a2dToArraySize();
        if (currentSize == -1) {
            printf("An error has occured.\n");
            break;
        }
        // long long rate = calcSortRate();
        // printf("Arrays being sorted at a rate of %llu/s\n", rate);
        updateSegments();
        Sorter_setArraySize(currentSize);
        printf("Setting array size to %d\n", currentSize);
        sleep(1);

    }

    pthread_exit(NULL);
}

static void updateSegments() {
    long long value = calcSortRate();
    
    if (value > 99) {
        value = 99;
    }
    char digits[3];
    sprintf(digits, "%lld", value);
    pthread_mutex_lock(&i2cMutex);
    if (value < 10) {
        currentDisplayedSegment.left = 0;
        currentDisplayedSegment.right = digits[0] - '0';
    }
    else {
        currentDisplayedSegment.left = digits[0] - '0';
        currentDisplayedSegment.right = digits[1] - '0';
    }

    if (currentDisplayedSegment.left > 9 || currentDisplayedSegment.right > 9 || currentDisplayedSegment.left < 0 || currentDisplayedSegment.right < 0) { // failsafe value checking
        currentDisplayedSegment.left = 0;
        currentDisplayedSegment.right = 0;
    }
    pthread_mutex_unlock(&i2cMutex);
}

static void changeFileAndWait(const char* file, int value) {
        FILE *pFile = fopen(file, "w");
        long seconds = 0;
        long nanoseconds = 3000000;
        struct timespec reqDelay = {seconds, nanoseconds};

        if (pFile == NULL) {
            printf("ERROR: unable to export file\n");
            fclose(pFile);
            nanosleep(&reqDelay, (struct timespec *) NULL);
            changeFileAndWait(file, value);
            return;
        }

        fprintf(pFile, "%d", value);
        fclose(pFile);
        nanosleep(&reqDelay, (struct timespec *) NULL);
}

void* i2cThread(void* args) {
    initI2C();
    // long seconds = 0;
    // long nanoseconds = 1000000;
    // struct timespec reqDelay = {seconds, nanoseconds};
    while (bbgStatus) {
        // 1. turn off both digits
        changeFileAndWait(LEFT_DIGIT_VALUE, 0);
        changeFileAndWait(RIGHT_DIGIT_VALUE, 0);
        // 2. Drive I2C GPIO extender to display pattern for left digit. Must write pattern to registers 0x14 and 0x15
        displayDigits(LEFT);
        //nanosleep(&reqDelay, (struct timespec *) NULL);
        // 3. Turn on left digit via GPIO 61 set to a 1. Wait for a little time
        changeFileAndWait(LEFT_DIGIT_VALUE, 1);
        // 4. Turn off both digits by driving a 0 to GPIO pins (Linux #'s 61 and 44).
        changeFileAndWait(LEFT_DIGIT_VALUE, 0);
        changeFileAndWait(RIGHT_DIGIT_VALUE, 0);
        // 5. Drive I2C GPIO extender to display pattern for right digit
        displayDigits(RIGHT);
        //nanosleep(&reqDelay, (struct timespec *) NULL);
        // 6. Turn on right digit via GPIO 44 set to a 1. Wait a little time.
        changeFileAndWait(RIGHT_DIGIT_VALUE, 1);
    }
    // turn off both digits when done.
    changeFileAndWait(LEFT_DIGIT_VALUE, 0);
    changeFileAndWait(RIGHT_DIGIT_VALUE, 0);
    pthread_exit(NULL);
}

static void displayDigits(int direction) {
    int digit = 0;
    pthread_mutex_lock(&i2cMutex);
    struct displaySegment cds = currentDisplayedSegment;
    pthread_mutex_unlock(&i2cMutex);
    if (direction == LEFT) {
        digit = cds.left;
    }
    else if (direction == RIGHT) {
        digit = cds.right;
    }
    else {
        printf("Invalid direction in display digits\n");
    }
    int bottomTopLed[2] = {0, 0};
    switch(digit){
		case 0:
			bottomTopLed[0] = 0xA1;
			bottomTopLed[1] = 0x86;
      		break;
		case 1:
			bottomTopLed[0] = 0x80;
			bottomTopLed[1] = 0x02;
      		break;
		case 2:
			bottomTopLed[0] = 0x31;
			bottomTopLed[1] = 0x0E;
      		break;
		case 3:
			bottomTopLed[0] = 0xB0;
			bottomTopLed[1] = 0x0E;
      		break;
		case 4:
			bottomTopLed[0] = 0x90;
			bottomTopLed[1] = 0x8A;
      		break;
		case 5:
			bottomTopLed[0] = 0xB0;
			bottomTopLed[1] = 0x8C;
      		break;
		case 6:
			bottomTopLed[0] = 0xB1;
			bottomTopLed[1] = 0x8C;
      		break;
		case 7:
			bottomTopLed[0] = 0x80;
			bottomTopLed[1] = 0x06;
      		break;
		case 8:
			bottomTopLed[0] = 0xB1;
			bottomTopLed[1] = 0x8E;
      		break;
		case 9:
			bottomTopLed[0] = 0x90;
			bottomTopLed[1] = 0x8E;
            break;
        default: //default is 0 for invalid values for error checking
            bottomTopLed[0] = 0xA1;
			bottomTopLed[1] = 0x86;
	}

    int i2cFileDesc = initI2cBus(I2CDRV_LINUX_BUS1, I2C_DEVICE_ADDRESS);

    writeI2cReg(i2cFileDesc, REG_OUTA, bottomTopLed[0]);
	writeI2cReg(i2cFileDesc, REG_OUTB, bottomTopLed[1]);

    close(i2cFileDesc);
}

// From the guide code.
static int initI2cBus(char* bus, int address)
{
	int i2cFileDesc = open(bus, O_RDWR);
	if (i2cFileDesc < 0) {
		printf("I2C DRV: Unable to open bus for read/write (%s)\n", bus);
		perror("Error is:");
		pthread_exit(NULL);
	}

	int result = ioctl(i2cFileDesc, I2C_SLAVE, address);
	if (result < 0) {
		perror("Unable to set I2C device to slave address.");
		pthread_exit(NULL);
	}
	return i2cFileDesc;
}

static void writeI2cReg(int i2cFileDesc, unsigned char regAddr, unsigned char value)
{
	unsigned char buff[2];
	buff[0] = regAddr;
	buff[1] = value;
	int res = write(i2cFileDesc, buff, 2);
	if (res != 2) {
		perror("Unable to write i2c register");
		exit(-1);
	}
}


void BBG_stopListen() {
    bbgStatus = false;
}