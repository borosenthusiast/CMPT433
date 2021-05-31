#include "threadmgr.h"


pthread_t sortThread;
pthread_attr_t sorterAttr;


pthread_t listenThread;
pthread_attr_t listenerAttr;


pthread_t potThread;
pthread_attr_t potAttr;

pthread_t ledThread;
pthread_attr_t ledAttr;


void Manager_startThreads() {
    printf("creating sorter thread\n");
    pthread_attr_init(&sorterAttr);
    pthread_create(&sortThread, &sorterAttr, Sorter_sorter, NULL);

    printf("creating listener thread\n");
    pthread_attr_init(&listenerAttr);
    pthread_create(&listenThread, &listenerAttr, Listener_listen, NULL);

    printf("creating potentiometer thread\n");
    pthread_attr_init(&potAttr);
    pthread_create(&potThread, &potAttr, BBG_listener, NULL);

    printf("creating led thread\n");
    pthread_attr_init(&ledAttr);
    pthread_create(&ledThread, &ledAttr, i2cThread, NULL);

    pthread_join(sortThread, NULL);
    pthread_join(listenThread, NULL);
    pthread_join(potThread, NULL);
    pthread_join(ledThread, NULL);

}

