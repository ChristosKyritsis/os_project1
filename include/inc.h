#include <semaphore.h>
#include <stdbool.h>

#define BUFFSIZE 1024

typedef struct {
    bool finished;
    sem_t semA;
    sem_t semB;
    char messageA[BUFFSIZE];
    char messageB[BUFFSIZE];
    int count;
    int numOfPieces;
    double waitingTime;
} SharedData;

void initialize_data(SharedData* data);
void free_data(SharedData* data);