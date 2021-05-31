/*Sorter module containing array sorter and interaction/data access.*/
#ifndef BUBBLE_H
#define BUBBLE_H
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdbool.h>

void* Sorter_sorter(void*);
void Sorter_requestComboData();
void Sorter_releaseComboData();
void Sorter_stopSorting();
long long Sorter_getNumSorted();
int* Sorter_getArrayData(int*);
void Sorter_setArraySize(int);
int Sorter_getValueAtOneIndex(int);
int Sorter_getArrayLength();



#endif