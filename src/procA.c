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
#include <stdbool.h>


#include "inc.h"

#define _POSIX_C_SOURCE 200809L

#define MAX_SIZE_OF_MESSAGE 15

#define SEM_PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP) // code from lab


int ftruncate(int fd, off_t length);


void* input_thread(void* arg) {
    SharedData* data = (SharedData*)arg;

    while (true) {
        printf("Please enter any message for Process B or type #BYE# to terminate the process:");
        fgets(data->messageA, MAX_SIZE_OF_MESSAGE, stdin);

        sem_post(&data->sem);

        if (strcmp(data->messageA, "#BYE#\n") == 0) {
            data->finished = true;
            sem_post(&data->sem);       // ending the process
            break;
        }
    }

    pthread_exit(NULL);
}


void* receive_thread(void* arg) {
    SharedData* data = (SharedData*)arg;
    struct timeval begin, end;

    while (true) {
        sem_wait(&data->sem);

        if (data->finished == true) 
            break;

        printf("Process B sent: %s\n", data->messageB);

        gettimeofday(&end, NULL);

        data->count++;
        data->numOfPieces += strlen(data->messageB);

        printf("Please enter any message for Process B or type #BYE# to terminate the process:");
        gettimeofday(&begin, NULL);
        fgets(data->messageA, MAX_SIZE_OF_MESSAGE, stdin);
        sem_post(&data->sem);

        if (strcmp(data->messageA, "#BYE#\n") == 0) {
            data->finished = true;
            sem_post(&data->sem);       // ending the process
            break;
        }
    }

    pthread_exit(NULL);
}


int main(int argc, char* argv[]) {
    // Creation of shared memory
    int fd;
    fd = shm_open(argv[1], O_CREAT | O_EXCL | O_RDWR, 0666);
    if (fd == -1) {
        printf("Error on opening shared memory segment\n");
        exit(EXIT_FAILURE);
    }
    
    int trunc;
    trunc = ftruncate(fd, sizeof(SharedData));
    if (trunc == -1) {
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