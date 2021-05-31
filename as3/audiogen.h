/*Module that uses the alsa api from the completed template and controls the audio playing loop*/
#ifndef AUDIOGEN_H
#define AUDIOGEN_H
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "gpio.h"
#include "network.h"
#include "audioMixer.h"

#define MAX_TEMPO 300
#define MIN_TEMPO 40
#define DEFAULT_TEMPO 120
#define MAX_VOLUME 100
#define MIN_VOLUME 0
#define DEFAULT_VOLUME 80


#define SND_BD_HARD "beatbox-wav-files/100051__menegass__gui-drum-bd-hard.wav"
#define SND_DRUM_CC "beatbox-wav-files/100053__menegass__gui-drum-cc.wav"
#define SND_SNARE_SOFT "beatbox-wav-files/100059__menegass__gui-drum-snare-soft.wav"
#define SND_CRASH "beatbox-wav-files/100061__menegass__gui-drum-splash-soft.wav"
#define SND_TOM_HI_HARD "beatbox-wav-files/100062__menegass__gui-drum-tom-hi-hard.wav"
#define SND_TOM_HI_SOFT "beatbox-wav-files/100063__menegass__gui-drum-tom-hi-soft.wav"

enum beats{STOP, ROCK, CUSTOM1, CUSTOM2};

enum instruments{BASE, HIHAT, SNARE};


void* Audio_Listener(void* args);
void Audio_StopListen();

void* Audio_Player(void* args);

void Audio_volumeChange(int);
void Audio_tempoChange(int);

int Audio_getVolume();
int Audio_getTempo();
int Audio_getBeat();
bool Audio_setBeat(int value);
bool Audio_getStatus();
void Audio_externalQueue(int);


#endif