#include "threadmgr.h"


pthread_t audioThread;
pthread_attr_t audioAttr;

pthread_mutex_t audioControllerMutex = PTHREAD_MUTEX_INITIALIZER;

int beat = NONE;
int tempo = DEFAULT_TEMPO;
int volume = DEFAULT_VOLUME;


wavedata_t base;
wavedata_t hihat;
wavedata_t snare;
wavedata_t crash;
wavedata_t tomHard;
wavedata_t tomSoft;

bool audioStatus = true;

static void cycleBeat();

static void initValues();
static void initAudio();

static void beatDelay();

static void stopBeat();
static void rockBeat();
static void custom1Beat();
static void custom2Beat();

static void initValues() {
    pthread_mutex_lock(&audioControllerMutex);
    volume = AudioMixer_getVolume();
    tempo = DEFAULT_TEMPO;
    beat = NONE;
    pthread_mutex_unlock(&audioControllerMutex);

}

static void initAudio() {
    AudioMixer_readWaveFileIntoMemory(SND_BD_HARD, &base);
	AudioMixer_readWaveFileIntoMemory(SND_DRUM_CC, &hihat);
	AudioMixer_readWaveFileIntoMemory(SND_SNARE_SOFT, &snare);
    AudioMixer_readWaveFileIntoMemory(SND_CRASH, &crash);
    AudioMixer_readWaveFileIntoMemory(SND_TOM_HI_HARD, &tomHard);
    AudioMixer_readWaveFileIntoMemory(SND_TOM_HI_SOFT, &tomSoft);
    printf("Starting Audio Thread\n");
    pthread_attr_init(&audioAttr);
	pthread_create(&audioThread, &audioAttr, Audio_Player, NULL);
}

static void beatDelay() {
    int currentTempo = Audio_getTempo();
    float calcNs = (60 / (float) currentTempo / 2) * 1000000000;
    //printf("currentTempo %d, calcNs %f\n", currentTempo, calcNs);
    long seconds = 0;
    long nanoseconds = (long) (calcNs); // wont exceed 32 bit limit even at min tempo.
    //printf("delay is %ld", nanoseconds);
    struct timespec reqDelay = {seconds, nanoseconds};
	nanosleep(&reqDelay, (struct timespec *) NULL);
}


static void stopBeat() {
    const int EIGHTH_BEAT = 8;
    for (int i = 0; i < EIGHTH_BEAT; i++) {
		beatDelay();
	}
}

static void rockBeat() {
    const int TIMING = 8;
    for (int i = 0; i < TIMING; i++) {
		if (i == 0 || i == 4) {
            AudioMixer_queueSound(&base);
        }
        else if (i == 2 || i == 6) {
            AudioMixer_queueSound(&snare);
        }
		AudioMixer_queueSound(&hihat);
		beatDelay();
	}
}

static void custom1Beat() {
    const int TIMING = 32;
    for (int i = 0; i < TIMING; i++) {
		if (i == 0) {
            AudioMixer_queueSound(&base);
            AudioMixer_queueSound(&crash);
        }
        else if (i == 4 || i == 12 || i == 20 || i == 28) {
            AudioMixer_queueSound(&tomHard);
        }
        else if (i == 10 || i == 16 || i == 26 || i == 29) {
            AudioMixer_queueSound(&base);
        }

        if (i > 1) {
            AudioMixer_queueSound(&hihat);
        }
        beatDelay();
	}
}

static void custom2Beat() {
    const int TIMING = 8;
    for (int i = 0; i < TIMING; i++) {
		if (i == 0) {
            AudioMixer_queueSound(&base);
        }
        else if (i == 4) {
            AudioMixer_queueSound(&snare);
        }

        AudioMixer_queueSound(&hihat);

        beatDelay();
	}
}

void* Audio_Player(void* args) {
    while (audioStatus) {
        int currentBeat = Audio_getBeat();
		switch (currentBeat) {
		case STOP:
			stopBeat();
			break;
		case ROCK:
			rockBeat();
			break;
		case CUSTOM1:
			custom1Beat();
			break;
        case CUSTOM2:
            custom2Beat();
            break;
        }
	}
    
    
    pthread_exit(0);
}

void* Audio_Listener(void* args) {
    printf("Starting GPIO listener thread.\n");
    initValues();
    exportGPIO();
    int option = NONE;
    initAudio();
    while (audioStatus) {
            option = pollJoyGPIO();
            //printf("option: %d\n", option);
            if (option == UP) {
                Audio_volumeChange(UP);

            }
            else if (option == DOWN) {
                Audio_volumeChange(DOWN);

            }
            else if (option == LEFT) {
                Audio_tempoChange(LEFT);

            }
            else if (option == RIGHT) {
                Audio_tempoChange(RIGHT);

            }
            else if (option == PUSH) {
                cycleBeat();           
            }

    }

    AudioMixer_freeWaveFileData(&base);
    AudioMixer_freeWaveFileData(&snare);
    AudioMixer_freeWaveFileData(&hihat);
    AudioMixer_cleanup();
    pthread_exit(0);
}

void Audio_externalQueue(int ins) {
    switch (ins) {
        case BASE:
            AudioMixer_queueSound(&base);
            break;
        case HIHAT:
            AudioMixer_queueSound(&hihat);
            break;
        case SNARE:
            AudioMixer_queueSound(&snare);
            break;
    }
}

void Audio_StopListen() {
    pthread_mutex_lock(&audioControllerMutex);
    audioStatus = false;
    pthread_mutex_unlock(&audioControllerMutex);
    

}

void Audio_tempoChange(int direction) {
    pthread_mutex_lock(&audioControllerMutex);
    if (direction == RIGHT) {
        tempo += 5;
    }
    else if (direction == LEFT) {
        tempo -= 5;
    }


    if (tempo < MIN_TEMPO) {
        tempo = MIN_TEMPO;
    }

    if (tempo > MAX_TEMPO) {
        tempo = MAX_TEMPO;
    }

    printf("tempo = %d\n", tempo);

    pthread_mutex_unlock(&audioControllerMutex);
    long seconds = 0;
    long nanoseconds = 300000000;
    struct timespec reqDelay = {seconds, nanoseconds};
    nanosleep(&reqDelay, (struct timespec *) NULL);
}

void Audio_volumeChange(int direction) {
    pthread_mutex_lock(&audioControllerMutex);
    if (direction == UP) {
        volume += 5;
    }
    else if (direction == DOWN) {
        volume -= 5;
    }
    


    if (volume < MIN_VOLUME) {
        volume = MIN_VOLUME;
    }

    if (volume > MAX_VOLUME) {
        volume = MAX_VOLUME;
    }

    printf("volume = %d\n", volume);
    AudioMixer_setVolume(volume);
    pthread_mutex_unlock(&audioControllerMutex);
    
    long seconds = 0;
    long nanoseconds = 300000000;
    struct timespec reqDelay = {seconds, nanoseconds};
    nanosleep(&reqDelay, (struct timespec *) NULL);
}

static void cycleBeat() {
    pthread_mutex_lock(&audioControllerMutex);
    if (beat == STOP) {
        printf("Beat set to ROCK\n");
        beat = ROCK;
    }
    else if (beat == ROCK) {
        printf("Beat set to CUSTOM1\n");
        beat = CUSTOM1;
    }
    else if (beat == CUSTOM1) {
        printf("Beat set to CUSTOM2\n");
        beat = CUSTOM2;
    }
    else if (beat == CUSTOM2) {
        printf("Beat set to STOP\n");
        beat = STOP;
    }
    pthread_mutex_unlock(&audioControllerMutex);
    long seconds = 0;
    long nanoseconds = 500000000;
    struct timespec reqDelay = {seconds, nanoseconds};
    nanosleep(&reqDelay, (struct timespec *) NULL);
}

bool Audio_setBeat(int value) {
    if (value >= NONE && value <= CUSTOM2) {
        pthread_mutex_lock(&audioControllerMutex);
        beat = value;
        pthread_mutex_unlock(&audioControllerMutex);

        return true;
    }

    return false;
}

int Audio_getVolume() {
    pthread_mutex_lock(&audioControllerMutex);
    int currentVolume = volume;
    pthread_mutex_unlock(&audioControllerMutex);
    return currentVolume;
}

int Audio_getTempo() {
    pthread_mutex_lock(&audioControllerMutex);
    int currentTempo = tempo;
    pthread_mutex_unlock(&audioControllerMutex);
    return currentTempo;
}

int Audio_getBeat() {
    pthread_mutex_lock(&audioControllerMutex);
    int currentBeat = beat;
    pthread_mutex_unlock(&audioControllerMutex);
    return currentBeat;
}

bool Audio_getStatus() {
    return audioStatus;
}