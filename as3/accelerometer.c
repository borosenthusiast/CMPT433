#include "accelerometer.h"


static int Accel_Init();
static int initI2cBus(char* bus, int address);
static void writeI2cReg(int i2cFileDesc, unsigned char regAddr, unsigned char value);
static int16_t* readI2cReg(int i2cFileDesc, unsigned char regAddr);

void* Accel_Listener(void* args) {
    int i2cFileDesc = Accel_Init();
    //int i2cFileDesc = initI2cBus(I2C1, I2C_ADDRESS);
    bool debounce;
    long seconds = 0;
    long nanoseconds = 10000000;
    struct timespec reqDelay = {seconds, nanoseconds};
    struct timespec reqDelayDebounce = {seconds, 10 * nanoseconds};
    while (Audio_getStatus()) {
        int16_t* axes = readI2cReg(i2cFileDesc, 0x00);
        // printf("axes[0] %" PRId16 "\n", axes[0]);
        // printf("axes[1] %" PRId16 "\n", axes[1]);
        // printf("axes[2] %" PRId16 "\n", axes[2]); // Y axis
        debounce = false;
        if (axes[0] >= XZTHRESHOLD || axes[0] <= -XZTHRESHOLD) {
            Audio_externalQueue(SNARE);
            debounce = true;
        }

        if (axes[1] >= XZTHRESHOLD || axes[1] <= -XZTHRESHOLD) {
            Audio_externalQueue(HIHAT);
            debounce = true;
        }

        if (axes[2] >= YDOWNTHRESHOLD || axes[2] <= -YUPTHRESHOLD) {
            Audio_externalQueue(BASE);
            debounce = true;
        }

        if (debounce) {
            nanosleep(&reqDelayDebounce, (struct timespec *) NULL);
        }


        free(axes);

        nanosleep(&reqDelay, (struct timespec *) NULL);
    }

    close(i2cFileDesc);
    pthread_exit(0);
}

static int Accel_Init() {
    int i2cFileDesc = initI2cBus(I2C1, I2C_ADDRESS);
    writeI2cReg(i2cFileDesc,I2C_ACTIVE,0x01);
    return i2cFileDesc;
    printf("accel init\n");
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

//from the guide code, modified for accelerometer
static int16_t* readI2cReg(int i2cFileDesc, unsigned char regAddr)
{
    const int XYZ = 3;
	// To read a register, must first write the address
	int res = write(i2cFileDesc, &regAddr, sizeof(regAddr));
	if (res != sizeof(regAddr)) {
		perror("Unable to write i2c register.");
		exit(-1);
	}

	// Now read the value and return it
	unsigned char value[BYTES_TO_READ];
	res = read(i2cFileDesc, &value, BYTES_TO_READ * sizeof(value[0]));
	if (res != sizeof(value)) {
		perror("Unable to read i2c register");
		exit(-1);
	}
    int16_t *axes;
    axes = calloc(XYZ, XYZ * sizeof(int16_t));
    //int16_t x = (buff[REG_XMSB] << 8) | (buff[REG_XLSB]);
    axes[0] = ((int)value[1] << 8) | ((int)value[2]);
    axes[1] = ((int)value[3] << 8) | ((int)value[4]);
    axes[2] = ((int)value[5] << 8) | ((int)value[6]);

	return axes;
}