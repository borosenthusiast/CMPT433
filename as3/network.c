#include "network.h"

pthread_mutex_t listenerMutex = PTHREAD_MUTEX_INITIALIZER;


bool listenStatus = true;

static void Listener_processBuffer(char buffIn[], int buffSize, char* reply);



static void Listener_processBuffer(char buffIn[], int buffSize, char* reply) {
    if (strncmp(buffIn, "volume ", 7) == 0 && buffSize > 7) {
        char subcommand[buffSize - 7];
        memset(subcommand, 0, sizeof(subcommand));
        memcpy(subcommand, &buffIn[7], sizeof(subcommand));
        // printf("ptr %s\n", ptr);
        //printf("subcommand %s\n", subcommand);
        if (strncmp(subcommand, "increase", 8) == 0) {
            Audio_volumeChange(UP);
            float uptime;
            FILE* uptimeFile = fopen("/proc/uptime", "r");
            fscanf(uptimeFile, "%f", &uptime);
            // csv format - uptime, volume, tempo, beat
            sprintf(reply, "bbgdata, %f, %d, %d, %d", uptime, Audio_getVolume(), Audio_getTempo(), Audio_getBeat());
            fclose(uptimeFile);
        }
        else if (strncmp(subcommand, "decrease", 8) == 0) {
            Audio_volumeChange(DOWN);
            float uptime;
            FILE* uptimeFile = fopen("/proc/uptime", "r");
            fscanf(uptimeFile, "%f", &uptime);
            // csv format - uptime, volume, tempo, beat
            sprintf(reply, "bbgdata, %f, %d, %d, %d", uptime, Audio_getVolume(), Audio_getTempo(), Audio_getBeat());
            fclose(uptimeFile);
        }
        else {
            sprintf(reply, "Invalid command recieved, %s\n", buffIn);
        }
    }
    else if (strncmp(buffIn, "tempo ", 6) == 0 && buffSize > 6) {
        char subcommand[buffSize - 6];
        memset(subcommand, 0, sizeof(subcommand));
        memcpy(subcommand, &buffIn[6], sizeof(subcommand));
        // printf("ptr %s\n", ptr);
        //printf("subcommand %s\n", subcommand);
        if (strncmp(subcommand, "increase", 8) == 0) {
            Audio_tempoChange(RIGHT);
            float uptime;
            FILE* uptimeFile = fopen("/proc/uptime", "r");
            fscanf(uptimeFile, "%f", &uptime);
            // csv format - uptime, volume, tempo, beat
            sprintf(reply, "bbgdata, %f, %d, %d, %d", uptime, Audio_getVolume(), Audio_getTempo(), Audio_getBeat());
            fclose(uptimeFile);
        }
        else if (strncmp(subcommand, "decrease", 8) == 0) {
            Audio_tempoChange(LEFT);
            float uptime;
            FILE* uptimeFile = fopen("/proc/uptime", "r");
            fscanf(uptimeFile, "%f", &uptime);
            // csv format - uptime, volume, tempo, beat
            sprintf(reply, "bbgdata, %f, %d, %d, %d", uptime, Audio_getVolume(), Audio_getTempo(), Audio_getBeat());
            fclose(uptimeFile);
        }
        else {
            sprintf(reply, "Invalid command recieved, %s\n", buffIn);
        }
    }
    else if (strncmp(buffIn, "beat ", 5) == 0 && buffSize > 5) {
        char subcommand[buffSize - 5];
        memset(subcommand, 0, sizeof(subcommand));
        memcpy(subcommand, &buffIn[5], sizeof(subcommand));
        // printf("ptr %s\n", ptr);
        //printf("subcommand %s\n", subcommand);
        if (strncmp(subcommand, "stop", 4) == 0) {
            Audio_setBeat(STOP);
            sprintf(reply, "Beat Changed to none.");
        }
        else if (strncmp(subcommand, "rock", 4) == 0) {
            Audio_setBeat(ROCK);
            sprintf(reply, "Beat Changed to Rock.");
        }
        else if (strncmp(subcommand, "custom1", 7) == 0) {
            Audio_setBeat(CUSTOM1);
            sprintf(reply, "Beat Changed to custom1.");
        }
        else if (strncmp(subcommand, "custom2", 7) == 0) {
            Audio_setBeat(CUSTOM2);
            sprintf(reply, "Beat Changed to custom2.");
        }
        else {
            sprintf(reply, "Invalid command recieved, %s\n", buffIn);
        }
    }
    else if (strncmp(buffIn, "play ", 5) == 0 && buffSize > 5) {
        char subcommand[buffSize - 5];
        memset(subcommand, 0, sizeof(subcommand));
        memcpy(subcommand, &buffIn[5], sizeof(subcommand));
        // printf("ptr %s\n", ptr);
        //printf("subcommand %s\n", subcommand);
        if (strncmp(subcommand, "snare", 5) == 0) {
            Audio_externalQueue(SNARE);
            float uptime;
            FILE* uptimeFile = fopen("/proc/uptime", "r");
            fscanf(uptimeFile, "%f", &uptime);
            // csv format - uptime, volume, tempo, beat
            sprintf(reply, "bbgdata, %f, %d, %d, %d", uptime, Audio_getVolume(), Audio_getTempo(), Audio_getBeat());
            fclose(uptimeFile);
        }
        else if (strncmp(subcommand, "hihat", 5) == 0) {
            Audio_externalQueue(HIHAT);
            float uptime;
            FILE* uptimeFile = fopen("/proc/uptime", "r");
            fscanf(uptimeFile, "%f", &uptime);
            // csv format - uptime, volume, tempo, beat
            sprintf(reply, "bbgdata, %f, %d, %d, %d", uptime, Audio_getVolume(), Audio_getTempo(), Audio_getBeat());
            fclose(uptimeFile);
        }
        else if (strncmp(subcommand, "base", 4) == 0) {
            Audio_externalQueue(BASE);
            float uptime;
            FILE* uptimeFile = fopen("/proc/uptime", "r");
            fscanf(uptimeFile, "%f", &uptime);
            // csv format - uptime, volume, tempo, beat
            sprintf(reply, "bbgdata, %f, %d, %d, %d", uptime, Audio_getVolume(), Audio_getTempo(), Audio_getBeat());
            fclose(uptimeFile);
        }
        else {
            sprintf(reply, "Invalid command recieved, %s\n", buffIn);
        }
    }
    else if (strncmp(buffIn, "check", 5) == 0) {
        float uptime;
        FILE* uptimeFile = fopen("/proc/uptime", "r");
        fscanf(uptimeFile, "%f", &uptime);
        // csv format - uptime, volume, tempo, beat
        sprintf(reply, "bbgdata, %f, %d, %d, %d", uptime, Audio_getVolume(), Audio_getTempo(), Audio_getBeat());
        fclose(uptimeFile);
    }
    else if (strncmp(buffIn, "stop", 4) == 0) {
        sprintf(reply, "Stop command issued. Stopping threads and freeing memory.\n");
        Audio_StopListen();
        Listener_stopListen();
    }
    else {
        sprintf(reply, "Invalid command recieved, %s\n", buffIn);
    }
    
}


void* Listener_listen(void* args) { // Templated from demo code.
    printf("Starting thread on UDP port %d\n", PORT);
    struct sockaddr_in sin;
    // Address
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;                   // Connection may be from network
	sin.sin_addr.s_addr = htonl(INADDR_ANY);    // Host to Network long
	sin.sin_port = htons(PORT);                 // Host to Network short
	
	// Create the socket for UDP
	int socketDescriptor = socket(PF_INET, SOCK_DGRAM, 0);

	// Bind the socket to the port (PORT) that we specify
	bind (socketDescriptor, (struct sockaddr*) &sin, sizeof(sin));
    while (listenStatus) {
		// Get the data (blocking)
		// Will change sin (the address) to be the address of the client.
		// Note: sin passes information in and out of call!
		struct sockaddr_in sinRemote;
		unsigned int sin_len = sizeof(sinRemote);
		char messageRx[MSG_MAX_LEN];
        memset(messageRx, 0, MSG_MAX_LEN);
		int bytesRx = recvfrom(socketDescriptor,
			messageRx, MSG_MAX_LEN, 0,
			(struct sockaddr *) &sinRemote, &sin_len);

		// Make it null terminated (so string functions work):
		int terminateIdx = (bytesRx < MSG_MAX_LEN) ? bytesRx : MSG_MAX_LEN - 1;
		messageRx[terminateIdx] = 0;
		//printf("Message received (%d bytes): \n\n'%s'\n", bytesRx, messageRx);
		// Compose the reply message:
		// (NOTE: watch for buffer overflows!).
		char messageTx[MSG_MAX_LEN];
        memset(messageTx, 0, MSG_MAX_LEN);
        //printf("first of buffer input %c\n", messageRx[0]);
        Listener_processBuffer(messageRx, terminateIdx, messageTx);
		// printf("%s\n", messageTx);
		// Transmit a reply:
        sin_len = sizeof(sinRemote);
        sendto( socketDescriptor,
            messageTx, strlen(messageTx),
            0,
            (struct sockaddr *) &sinRemote, sin_len);

	}

	// Close
	close(socketDescriptor);
    pthread_exit(0);
}


void Listener_stopListen() {
    pthread_mutex_lock(&listenerMutex);
    listenStatus = false;
    pthread_mutex_unlock(&listenerMutex);
}



