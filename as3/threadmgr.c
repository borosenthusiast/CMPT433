#include "threadmgr.h"


pthread_t joyThread;
pthread_attr_t joyAttr;

pthread_t networkThread;
pthread_attr_t networkAttr;

pthread_t accelThread;
pthread_attr_t accelAttr;

void Manager_startThreads() {
    AudioMixer_init();
    printf("creating joy thread\n");
    pthread_attr_init(&joyAttr);
    pthread_create(&joyThread, &joyAttr, Audio_Listener, NULL);

    printf("creating network thread\n");
    pthread_attr_init(&networkAttr);
    pthread_create(&networkThread, &networkAttr, Listener_listen, NULL);

    printf("creating accelerometer thread\n");
    pthread_attr_init(&accelAttr);
    pthread_create(&accelThread, &accelAttr, Accel_Listener, NULL);

    pthread_join(joyThread, NULL);
    pthread_join(networkThread, NULL);
    pthread_join(accelThread, NULL);

}

