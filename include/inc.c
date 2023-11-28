#include "inc.h"


void initialize_data(SharedData* data) {
    sem_init(&data->semA, 1, 0);        // second argument is 1 because the semaphores are shared
    sem_init(&data->semB, 1, 0);        // between two processes
    sem_init(&data->terminatingSem, 1, 0);
    data->countA = 0;
    data->countB = 0;                    
    data->numOfPiecesA = 0;
    data->numOfPiecesB = 0;
    data->finished = false;
    data->waitingTimeA = 0.0;
    data->waitingTimeB = 0.0;
}


void print_data(SharedData* data) {
    if (data != NULL) {
        printf("Number of messages process A has sent: %d\n", data->countA);
        printf("Number of messages process A has received: %d\n", data->countB);
        printf("Number of messages process B has sent: %d\n", data->countB);
        printf("Number of messages process A has received: %d\n", data->countA);
    }
    else {
        printf("Empty\n");
    }
}


void free_data(SharedData* data) {
    sem_destroy(&data->semA);
    sem_destroy(&data->semB);
    sem_destroy(&data->terminatingSem);
}