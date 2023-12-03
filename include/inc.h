#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h> // Creating and managing shared memory
#include <unistd.h> // 
#include <sys/types.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/stat.h> // for (S_IRUSR|S_IWUSR)
#include <sys/mman.h>
#include <string.h>
#include <pthread.h> // Used for creating and managing threads
#include <sys/time.h>
#include <semaphore.h>
#include <stdbool.h>

#define BUFFSIZE 4096

typedef struct {
    bool finished;
    sem_t semA;
    sem_t semB;
    char messageA[BUFFSIZE];
    char messageB[BUFFSIZE];
    int countA;
    int countB;
    int numOfPiecesA;
    int numOfPiecesB;
    double waitingTimeA;
    double waitingTimeB;
} SharedData;

void initialize_data(SharedData* data);
void print_data(SharedData* data);
void free_data(SharedData* data);