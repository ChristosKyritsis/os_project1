#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h> // Functions that are semaphore-related
#include <sys/ipc.h> // Creating and managing shared memory
#include <sys/shm.h> // Creating and managing shared memory
#include <sys/stat.h> // for (S_IRUSR|S_IWUSR)
#include <string.h>
#include <pthread.h> // Used for creating and managing threads
#include <time.h>
#include <stdint.h>

#define MAX_SIZE_OF_MESSAGE 15
#define BUFFSIZE 4096
#define SEM_PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP) // code from lab

sem_t* mutex;
sem_t* count;
char* bufferA;
char* bufferB;

void* inputThreadB(void* item) {
    // char* childID;
    // childID = (char*)item;

    //printf("We are in process %s", childID);
    
    int shmid;

    // Creating Shared Memory Segment
    shmid = shmget(IPC_PRIVATE, sizeof(sem_t), (S_IRUSR|S_IWUSR));
    if (shmid == -1) {
        perror("Creation of Shared Memory FAILED\n");
        exit(EXIT_FAILURE);
    }

    // Ataching Memory Segment
    void* address = shmat(shmid, NULL, 0);
    if (address == (void*)-1) {
        printf("Failed to attach memory segment\n");
        exit(EXIT_FAILURE);
    }


    mutex = (sem_t*)address;
    count = mutex + 1;
    char* bufferB = (char*)(count + 1);

    // sem_init(mutex, 1, 1);
    // sem_init(count, 1, 0);

    while(1) {
        printf("Type any message to continue, and type #BYE# if you want to terminate the process\n");
        char input[MAX_SIZE_OF_MESSAGE];
        fgets(input, MAX_SIZE_OF_MESSAGE, stdin);

        sem_wait(mutex);

        if (!strcmp(input, "#BYE#")) {
            // Releasing the semaphore
            sem_post(mutex);

            sem_wait(mutex);
            strcpy(bufferB, "#BYE#");
            //sem_post(mutex);
            sem_post(count);
            break;
        }
        strncpy(bufferB, input, MAX_SIZE_OF_MESSAGE);
        // Updating the count
        sem_post(count);

        sem_post(mutex);
    }
    int del;
    del = shmdt(mutex);

    if (del == -1) {
        printf("Deletion of shared memory failed\n");
        return NULL;
    }
    //

    pthread_exit(NULL);
}

void* receiveThreadB(void* item) {
    // char *childID;
    // childID = (char *)item;

    int shmid;

    // Creating Shared Memory Segment
    shmid = shmget(IPC_PRIVATE, sizeof(sem_t), (S_IRUSR|S_IWUSR));
    if (shmid == -1) {
        perror("Creation of Shared Memory FAILED\n");
        exit(EXIT_FAILURE);
    }

    // Ataching Memory Segment
    void* address = shmat(shmid, NULL, 0);
    if (address == (void*)-1) {
        printf("Failed to attach memory segment\n");
        exit(EXIT_FAILURE);
    }


    mutex = (sem_t*)address;
    count = mutex + 1;
    char* bufferB = (char*)(count + 1);

    sem_init(mutex, 1, 1);
    sem_init(count, 1, 0);


    while (1) {
        sem_wait(mutex);
        sem_wait(count);

        if (!strcmp(bufferB, "#BYE#")) {
            sem_post(mutex);
            break;
        }

        printf("Process B Received message: %s\n", bufferB);

        sem_post(mutex);
    }
    int del;
    del = shmdt(mutex);

    if (del == -1) {
        printf("Deletion of shared memory failed\n");
        return NULL;
    }
    //

    pthread_exit(NULL);
}