#include "inc.h"


void initialize_data(SharedData* data) {
    sem_init(&data->semA, 1, 0);        // second argument is 1 because the semaphores are shared
    sem_init(&data->semB, 1, 0);        // between two processes
    data->count = 0;                    
    data->numOfPieces = 0;
    data->finished = false;
    data->waitingTime = 0.0;
}

void free_data(SharedData* data) {
    sem_destroy(&data->semA);
    sem_destroy(&data->semB);
}