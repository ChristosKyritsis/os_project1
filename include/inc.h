#include <semaphore.h>
#include <stdbool.h>

#define BUFFSIZE 4096

typedef struct {
    bool finished;
    sem_t sem;
    char message[BUFFSIZE];
    int count;
    int numOfPieces;
    double waitingTime;
} SharedData;

void initialize_data(SharedData* data);
void free_data(SharedData* data);