#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h> // Creating and managing shared memory
#include <unistd.h> // ftruncate  
#include <sys/types.h> // ftruncate
#include <fcntl.h> // O_CREAT etc.
#include <sys/mman.h>
#include <string.h>
#include <pthread.h> // Used for creating and managing threads
#include <sys/time.h> 
#include <semaphore.h>
#include <stdbool.h> // only used for while(true)

#define BUFFSIZE 4096
#define MAX_SIZE_OF_MESSAGE 15

typedef struct {
    sem_t semA;
    sem_t semB;
    char messageA[BUFFSIZE];    // Stores the message that Process A sends
    char messageB[BUFFSIZE];    // Stores the message that Process B sends
    int countA;                 // Number of messages that Process A has sent
    int countB;                 // Number of messages that Process B has sent
    int numOfPiecesA;           // Size of the messages that Process B has sent to Process A
    int numOfPiecesB;           // Size of the messages that Process A has sent to Process B
    double waitingTimeA;
    double waitingTimeB;
} SharedData;

void initialize_data(SharedData* data);
void print_data(SharedData* data);
void free_data(SharedData* data);