/*Module that initializes the threads for each of the different modules. Thread stopping is handled by network*/
#ifndef THREADMGR_H
#define THREADMGR_H

#include "audiogen.h"
#include "network.h"
#include "accelerometer.h"

void Manager_startThreads();

#endif