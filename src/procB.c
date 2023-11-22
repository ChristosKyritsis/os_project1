#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h> // Functions that are semaphore-related
#include <sys/ipc.h> // Creating and managing shared memory
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/shm.h> // Creating and managing shared memory
#include <sys/stat.h> // for (S_IRUSR|S_IWUSR)
#include <string.h>
#include <pthread.h> // Used for creating and managing threads
#include <sys/time.h>

#include "inc.h"

#define MAX_SIZE_OF_MESSAGE 15
#define BUFFSIZE 4096
#define SEM_PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP) // code from lab might delete later


void* input_thread(void* arg) {
    SharedData* data = (SharedData*)arg;

    while (true) {
        printf("Process A sent: %s", data->messageA);
        printf("Please enter any message for Process A or type #BYE# to terminate the process:");

        fgets(data->messageB, MAX_SIZE_OF_MESSAGE, stdin);
        sem_post(&data->sem);

        if (strcmp(data->messageB, "#BYE#\n") == 0) {
            data->finished = true;
            sem_post(&data->sem);
            break;
        }
    }

    pthread_exit(NULL);
}


void* receive_thread(void* arg) {
    SharedData* data = (SharedData*)arg;
    struct timeval begin, end;
    double totalTime = 0.0;

    while (true) {
        sem_wait(&data->sem);

        if (data->finished == true)
            break;

        printf("Process A sent: %s", data->messageA);
        
        gettimeofday(&end, NULL);
        totalTime = end.tv_sec - begin.tv_sec;

        data->count++;
        data->numOfPieces +=strlen(data->messageA);
        data->waitingTime += totalTime;

        printf("Please enter any message for Process A or type #BYE# to terminate the process:");
        gettimeofday(&begin, NULL);
        fgets(data->messageB, MAX_SIZE_OF_MESSAGE, stdin);
        sem_post(&data->sem);

        if (strcmp(data->messageB, "#BYE#\n") == 0) {
            data->finished = true;
            sem_post(&data->sem);
            break;
        }

    }

    pthread_exit(NULL);
}


int main(int argc, char* argv[]) {
    int fd;
    fd = shm_open(argv[1], O_RDWR, 0666);
    if (fd == -1) {
        printf("Failed to open shared memory\n");
        exit(EXIT_FAILURE);
    }

    SharedData* data = (SharedData*)mmap(NULL, sizeof(SharedData), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (data == MAP_FAILED) {
        exit(EXIT_FAILURE);
    }

    initialize_data(data);

    int res1, res2;
    pthread_t inpThread, recThread;

    res1 = pthread_create(&recThread, NULL, receive_thread, (void*)data);
    if (res1 != 0) {
        printf("Creation of thread failed\n");
        exit(EXIT_FAILURE);
    }

  
    res2 = pthread_create(&inpThread, NULL, input_thread, (void*)data);

    if (res2 != 0) {
        printf("Creation of thread failed\n");
        exit(EXIT_FAILURE);
    }

    pthread_join(inpThread, NULL);
    pthread_join(recThread, NULL);

    free_data(data);
    munmap(data, sizeof(SharedData));
    unlink(argv[1]);

    return 0;
}