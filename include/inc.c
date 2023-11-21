#include "inc.h"


void initialize_data(SharedData* data) {
    sem_init(&data->sem, 1, 0);
    data->count = 0;
    data->numOfPieces = 0;
    data->finished = false;
    data->waitingTime = 0.0;
}

void free_data(SharedData* data) {
    sem_destroy(&data->sem);
}