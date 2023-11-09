#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h> // Functions that are semaphore-related
#include <sys/ipc.h> // Creating and managing shared memory
#include <sys/shm.h> // Creating and managing shared memory
#include <string.h>
#include <pthread.h> // Used for creating and managing threads

#include "thread_func.h"

#define MAX_SIZE_OF_MESSAGE 15

struct Message {
    char msg[MAX_SIZE_OF_MESSAGE];
    int size;
};

struct SharedMemory{
    sem_t sem;
    int count; // Number of messsages
    struct Message message;
};

struct SharedMemory* sharedMemory;


enum ProcessType {
    A,
    B
};

char message[] = "Hello World";

int main(int argc, char** argv) {
    int resInp, resRec;
    pthread_t inputThread, receiveThread;

    //resInp = pthread_create(&inputThread, NULL, input, )


    // if (argc == 1) {
    //     printf("No arguments were given\n");
    // }
    // else {
    //     printf(argv[1], "\n");
    // }
    return 0;
}