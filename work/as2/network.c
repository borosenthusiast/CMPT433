#include "network.h"

pthread_mutex_t listenerMutex = PTHREAD_MUTEX_INITIALIZER;


bool listenStatus = true;
long long value = -1;


static char* intArrayToString(const int* array, int currentLength, bool* remainder, int* startingIndex);
static void Listener_processBuffer(char buffIn[], int buffSize, char* reply, int* arrayValues);
static int checkNumber(const char* str, int size);
static int getNumDigits(int number);


static int checkNumber(const char* str, int size) {
    int status = 1; // has nondigits
    for (int i = 0; i < size - 1; i++) { // account for null character
        if (isdigit(str[i]) == 0) {
            status = 1;
            break;
        }
        else {
            status = 0;
        }
    }

    return status;
}

static int getNumDigits(int number) {
    int digits = 0;
    while (number != 0) 
    {
        number = number / 10;
        digits++;
    }
    return digits;

}

void printArray(int* array, int length) {
    printf("\n");
    for (int i = 0; i < length; i++) {
        printf("%d\t", array[i]);
        if (i % 10 == 0 && array[i] != 0 && i != 0) {
            printf(".\n");
        }
        else if (array[i] == 0) {
            printf("there has been a mistake at index %d\n", i);
        }
    }
    printf("\n");
}


static void Listener_processBuffer(char buffIn[], int buffSize, char* reply, int* arrayValues) {
    if (strncmp(buffIn, "get ", 4) == 0 && buffSize > 4) {
        char subcommand[buffSize - 4];
        memset(subcommand, 0, sizeof(subcommand));
        memcpy(subcommand, &buffIn[4], sizeof(subcommand));
        char* ptr;
        long long getDigit = strtol(subcommand, &ptr, 10);
        // printf("ptr %s\n", ptr);
        //printf("subcommand %s\n", subcommand);
        if (strncmp(subcommand, "length\n", 7) == 0 || strncmp(subcommand, "length\0", 7) == 0 ) {
            int arrLen = Sorter_getArrayLength();
            sprintf(reply, "The current array length is %d.\n", arrLen);
        }
        else if (strncmp(subcommand, "array", 5) == 0 || strncmp(subcommand, "array\0", 6) == 0 ) {
            Sorter_getArrayData(arrayValues);
            //memcpy(arrayValues, Sorter_getArrayData(), sizeof(int) * Sorter_getArrayLength());
            sprintf(reply, "GETARRAY");
        }
        else if (checkNumber(subcommand, buffSize-4) == 0) { // TODO: 1-indexed get command.
            if (getDigit < 1) {
                sprintf(reply, "Get %lld is below minimum index 1.\n", getDigit);
            }
            else if (getDigit > Sorter_getArrayLength()) {
                sprintf(reply, "Get %lld is above maximum index %d.\n", getDigit, Sorter_getArrayLength());
            }
            else {
                sprintf(reply, "Value at %lld is %d.\n", getDigit, Sorter_getValueAtOneIndex(getDigit));
            }
            
        }
        else {
            sprintf(reply, "Invalid get parameter, see help for get parameters\n");
        }
    }
    else if (strncmp(buffIn, "count\n", 6) == 0 || strncmp(buffIn, "count\0", 6) == 0){
        long long count = Sorter_getNumSorted();
        sprintf(reply, "There have been %lld sorted arrays.\n", count);
    }
    else if (strncmp(buffIn, "help", 4) == 0) {
        sprintf(reply, "Commands are: count, get X, stop, and help.\nCount gets the number of arrays sorted\nGet X returns the Xth element of the array (1-indexed), get array returns the array in full, and get length returns the length of the array being sorted.\nStop shuts down the program\nHelp lists options available to the user.\n");
    }
    else if (strncmp(buffIn, "stop", 4) == 0) {
        sprintf(reply, "Stop command issued. Stopping threads and freeing memory.\n");
        Sorter_stopSorting();
        BBG_stopListen();
        Listener_stopListen();
    }
    else {
        sprintf(reply, "Invalid command issued, type \"help\" for list of options.\n");
    }
    
}



static char* intArrayToString(const int* array, int currentLength, bool* remainder, int* startingIndex) {
    char* arrayReply;
    long long lengthOfChars = 0;
    int i = *startingIndex;
    if (i >= currentLength) {
        printf("i greater than currentLength\n");
        *remainder = false;
        return "\n";
    }
    for (;i < currentLength; i += 1) {
        lengthOfChars += getNumDigits(array[i]) + 2; // comma and space
        
        if (i + 1 < currentLength) {
            if (sizeof(char) * (lengthOfChars + getNumDigits(array[i + 1] + 2)) > MSG_MAX_LEN) {
                i -= 1;
                break;
            }
        }
        else if (sizeof(char) * lengthOfChars > MSG_MAX_LEN) {
            i -= 1;
            break;
        }
    }

    arrayReply = calloc(lengthOfChars, sizeof(char) * lengthOfChars);
    for (int j = *startingIndex; j < i; j++) {
        if (j == currentLength - 1) {
            //printf("arrval:%d  arridx:%d\n", array[*startingIndex + j], *startingIndex);
            sprintf(arrayReply + strlen(arrayReply), "%d\n", array[j]);
            *remainder = false; // no more numbers to fit inside the packet
        }
        else {
            //printf("arrval:%d  arridx:%d\n", array[*startingIndex + j], *startingIndex);
            sprintf(arrayReply + strlen(arrayReply), "%d, ", array[j]);
            *remainder = true;
        }
        
    }
    //printf("arrayReply length: %lu\n", strlen(arrayReply));
    *startingIndex = i;
    return arrayReply;

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
        Sorter_requestComboData();
        int currentLength = Sorter_getArrayLength();
        int* arrayValues = malloc(sizeof(int) * currentLength);
        Listener_processBuffer(messageRx, terminateIdx, messageTx, arrayValues);
        Sorter_releaseComboData();
		// printf("%s\n", messageTx);
		// Transmit a reply:
        if (strncmp(messageTx, "GETARRAY", 8) != 0) {
            sin_len = sizeof(sinRemote);
            sendto( socketDescriptor,
                messageTx, strlen(messageTx),
                0,
                (struct sockaddr *) &sinRemote, sin_len);
        }
        else {
            bool remainder = true;
            int startIndex = 0;
            while (remainder) {
                char* reply = intArrayToString(arrayValues, currentLength, &remainder, &startIndex);
                //printf("startindex %d\n", startIndex);
                sin_len = sizeof(sinRemote);
                sendto( socketDescriptor,
                    reply, strlen(reply),
                    0,
                    (struct sockaddr *) &sinRemote, sin_len);

                free(reply);
                }
        }
        free(arrayValues);
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



