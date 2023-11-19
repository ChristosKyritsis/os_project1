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

void* inputThreadB(void* item);
void* receiveThreadB(void* item);




// typedef struct {
//     char msg[BUFSIZE];
//     int size;
// } Message;

// typedef struct {
//     sem_t mutex;
//     sem_t count;
//     Message message;
// } SharedMemory;

// SharedMemory *sharedMem;

    sem_t* mutex;
    sem_t* count;
    char* bufferA;
    char* bufferB;

void* inputThread(void* item) {
    // char* childID;
    // childID = (char*)item;

    //printf("We are in process %s\n", childID);
    
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
    char* bufferA = (char*)(count + 1);

    sem_init(mutex, 1, 1);
    sem_init(count, 1, 0);

    while(1) {
        printf("Type any message to continue, and type #BYE# if you want to terminate the process\n");
        char input[MAX_SIZE_OF_MESSAGE];
        fgets(input, MAX_SIZE_OF_MESSAGE, stdin);

        sem_wait(mutex);

        if (!strcmp(input, "#BYE#")) {
            // Releasing the semaphore
            sem_post(mutex);

            sem_wait(mutex);
            strcpy(bufferA, "#BYE#");
            //sem_post(mutex);
            sem_post(count);
            break;
        }
        strncpy(bufferA, input, MAX_SIZE_OF_MESSAGE);
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


void* receiveThread(void* item) {
    // char *childID;
    // childID = (char *)item;

    int shmid;
    //Creating Shared Memory Segment
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
    char* bufferA = (char*)(count + 1);

    sem_init(mutex, 1, 1);
    sem_init(count, 1, 0);


    while (1) {
        sem_wait(mutex);
        sem_wait(count);

        if (strcmp(bufferA, "#BYE#\n") == 0) {
            sem_post(mutex);
            break;
        }

        printf("Process A received message: %s\n", bufferA);

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


int main(int argc, char* argv[]) {
    if (argc == 1) {
        printf("No arguments were given\n");
    }
    else {
        for (int i = 1; i < argc; i++)
            printf("argument  %d given is : %s \n", i, argv[i]);
    }

    int shmid;

    // Creating Shared Memory Segment with size for two semaphors and two buffers
    shmid = shmget(IPC_PRIVATE, sizeof(sem_t) + sizeof(sem_t) + BUFFSIZE + BUFFSIZE, (S_IRUSR|S_IWUSR));
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

    sem_init(mutex, 1/*shared across processes*/, 1);
    sem_init(count, 1, 0);

    pthread_t recThread;
    int res;

    res = pthread_create(&recThread, NULL, receiveThread, (void*)address);
    if (res != 0) {
        printf("Creation of thread failed\n");
        exit(EXIT_FAILURE);
    }

    pthread_t inpThread;
    int res2;

    res2 = pthread_create(&inpThread, NULL, inputThread, (void*)address);

    if (res2 != 0) {
        printf("Creation of thread failed\n");
        exit(EXIT_FAILURE);
    }


    pthread_t recThreadB;

    int resB1;
    resB1 = pthread_create(&recThreadB, NULL, receiveThreadB, (void*)address);
    if (resB1 != 0) {
        printf("Creation of thread failed\n");
        exit(EXIT_FAILURE);
    }

    pthread_t inpThreadB;
    int resB2;

    resB2 = pthread_create(&inpThreadB, NULL, inputThreadB, (void*)address);

    if (resB2 != 0) {
        printf("Creation of thread failed\n");
        exit(EXIT_FAILURE);
    }


    pthread_join(inpThread, NULL);
    pthread_join(recThread, NULL);

    int del;
    del = shmdt(mutex);

    if (del == -1) {
        printf("Deletion of shared memory failed\n");
        return -1;
    }
    //
    

    return 0;
}