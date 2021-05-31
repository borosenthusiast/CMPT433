#include "bubble.h"


long long numSorted = 0;
int* array;
int arrayLength = 1000; //default 1000
bool sortStatus = true;
int nextLength = 0;
bool lengthChanged = false;
pthread_mutex_t sorterMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t dataRequestMutex = PTHREAD_MUTEX_INITIALIZER;

static int initArray(int length, int* array);


static int initArray(int length, int* array) {
    
    time_t rand_time;
    srand((unsigned) time(&rand_time));
    for (int i = 0; i < length; i++) {
        array[i] = i + 1;
    }
    for (int i = 0; i < length; i++) {
        int dest_idx = rand() % (i + 1);
        int temp;
        temp = array[i];
        array[i] = array[dest_idx];
        array[dest_idx] = temp;
        
    }

    return 1;
}

void* Sorter_sorter(void* args) {
    printf("Starting sorter function\n");
    pthread_mutex_lock(&sorterMutex);
    bool currentsortStatus = sortStatus;
    pthread_mutex_unlock(&sorterMutex);
    while (currentsortStatus) { // run continously while currentsortStatus is true
        pthread_mutex_lock(&sorterMutex);
        int currentLength = arrayLength;
        free(array);
        array = (int*)malloc(currentLength * sizeof(int));
        initArray(currentLength, array);
        pthread_mutex_unlock(&sorterMutex);
        
        
        for (int i = 0; i < currentLength - 1; i++) {
            for (int j = 0; j < currentLength - i - 1; j++) {
                pthread_mutex_lock(&sorterMutex);
                if (array[j] > array[j+1]) {
                    int temp = array[j];
                    array[j] = array[j+1];
                    array[j+1] = temp;
                }
                pthread_mutex_unlock(&sorterMutex);
            }
        }
        
        pthread_mutex_lock(&sorterMutex);
        //printf("Sorter thread: sorting completed\n");
        numSorted++;
        currentsortStatus = sortStatus;
        if (lengthChanged) {
            pthread_mutex_lock(&dataRequestMutex);
            arrayLength = nextLength;
            lengthChanged = false;
            pthread_mutex_unlock(&dataRequestMutex);
        }
        //printf("sorted array\n");
        pthread_mutex_unlock(&sorterMutex);
    }
    free(array);
    pthread_exit(NULL);
}

void Sorter_requestComboData() {
    pthread_mutex_lock(&dataRequestMutex);
}

void Sorter_releaseComboData() {
    pthread_mutex_unlock(&dataRequestMutex);
}


void Sorter_stopSorting() {
    pthread_mutex_lock(&sorterMutex);
    sortStatus = false;
    pthread_mutex_unlock(&sorterMutex);
}

long long Sorter_getNumSorted() {
    pthread_mutex_lock(&sorterMutex);
    long long sorted = numSorted;
    pthread_mutex_unlock(&sorterMutex);
    return sorted;
    
}

void Sorter_setArraySize(int newSize) {
    if (newSize < 1) { //failsafe
        newSize = 1;
    }
    pthread_mutex_lock(&sorterMutex);
    nextLength = newSize;
    lengthChanged = true;
    pthread_mutex_unlock(&sorterMutex);
}

int* Sorter_getArrayData(int* newArray) {
    pthread_mutex_lock(&sorterMutex);
    memcpy(newArray, array, sizeof(int) * arrayLength);
    pthread_mutex_unlock(&sorterMutex);
    return newArray;
}

int Sorter_getArrayLength() {
    pthread_mutex_lock(&sorterMutex);
    int arrayLen = arrayLength;
    pthread_mutex_unlock(&sorterMutex);
    return arrayLen;
}

int Sorter_getValueAtOneIndex(int oneIndex) {
    int currMax = Sorter_getArrayLength();
    if (oneIndex > currMax) {
        return -1; //this shouldnt happen
    }
    else {
        pthread_mutex_lock(&sorterMutex);
        int value = array[oneIndex - 1];
        pthread_mutex_unlock(&sorterMutex);
        return value;
    }
}

