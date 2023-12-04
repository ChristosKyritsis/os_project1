#include "inc.h"


void initialize_data(SharedData* data) {
    // second argument is 1 because the semaphores are shared between the two processes
    if (sem_init(&data->semA, 1, 0) == -1) {
        printf("Failed to open semaphore\n");
        exit(EXIT_FAILURE);    
    }  
    if (sem_init(&data->semB, 1, 0) == -1) {
        printf("Failed to open semaphore\n");
        exit(EXIT_FAILURE);
    } 
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
        printf("\n-----END OF PROCESS-----\n");
        printf("\n\n-----STATISTICS-----\n\n");
        printf("Number of messages process A has sent: %d\n", data->countA);  
        printf("Number of messages process A has received: %d\n", data->countB);
        printf("Number of messages process B has sent: %d\n", data->countB);
        printf("Number of messages process B has received: %d\n", data->countA);
        printf("Total number of pieces in process A: %d\n", data->numOfPiecesA);    
        printf("Total number of pieces in process B: %d\n", data->numOfPiecesB);
        printf("Average number of pieces in process A: %.1f\n", (double)data->numOfPiecesA/data->countA);
        printf("Average number of pieces in process B: %.1f\n", (double)data->numOfPiecesB/data->countB);
        printf("Total waiting time in procees A: %.6f\n", data->waitingTimeA);
        printf("Total waiting time in procees B: %.6f\n", data->waitingTimeB);
        printf("Average waiting time in process A: %.6f\n", data->waitingTimeA/data->countA);
        printf("Average waiting time in process B: %.6f\n", data->waitingTimeB/data->countB);
    }
    else {
        printf("ERROR!\n");
    }
}


void free_data(SharedData* data) {
    sem_destroy(&data->semA);
    sem_destroy(&data->semB);
}