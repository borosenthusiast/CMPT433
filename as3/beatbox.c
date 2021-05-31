#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "threadmgr.h"

int main() {
    printf("Main Function\n");
    Manager_startThreads();
    printf("Threads have been stopped, exiting program.\n");
    return 0;
}