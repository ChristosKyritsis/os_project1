#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h> // Functions that are semaphore-related
#include <sys/ipc.h> // Creating and managing shared memory
#include <sys/shm.h> // Creating and managing shared memory
#include <sys/stat.h> // for (S_IRUSR|S_IWUSR)
#include <string.h>
#include <pthread.h> // Used for creating and managing threads
#include <time.h>

#define MAX_SIZE_OF_MESSAGE 15
#define BUFSIZE 1024
#define SEM_PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP) // code from lab

// void processB(int i, void* args);


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


void* input_Thread(void* item) {
    char* childID;
    childID = (char*)item;

    printf("We are in process %s", childID);

    sem_t* mutex;
    sem_t* count;
    int shmid;

    // Creating Shared Memory Segment
    shmid = shmget(IPC_PRIVATE, sizeof(sem_t), (S_IRUSR|S_IWUSR));
    if (shmid == -1) {
        perror("Creation of Shared Memory FAILED\n");
        exit(EXIT_FAILURE);
    }

    // Ataching Memory Segment
    mutex = (sem_t*)shmat(shmid, NULL, 0);
    count = mutex + 1;
    char* message = (char*)(count + 1);

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
            strcpy(message, "#BYE#");
            //sem_post(mutex);
            sem_post(count);
            break;
        }
        strncpy(message, input, MAX_SIZE_OF_MESSAGE);
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


void *receive_Thread(void *item) {
    char *childID;
    childID = (char *)item;

    printf("We are in process %s", childID);

    sem_t* mutex;
    sem_t* count;
    int shmid;

    // Creating Shared Memory Segment
    shmid = shmget(IPC_PRIVATE, sizeof(sem_t), (S_IRUSR|S_IWUSR));
    if (shmid == -1) {
        perror("Creation of Shared Memory FAILED\n");
        exit(EXIT_FAILURE);
    }

    // Ataching Memory Segment
    mutex = (sem_t*)shmat(shmid, NULL, 0);
    count = mutex + 1;
    char* message = (char*)(count + 1);

    sem_init(mutex, 1, 1);
    sem_init(count, 1, 0);


    while (1) {
        sem_wait(mutex);
        sem_wait(count);

        if (!strcmp(message, "#BYE#")) {
            sem_post(mutex);
            break;
        }

        printf("Received message: %s\n", message);

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
            printf("argument  %d given is: %s \n", i, argv[i]);
    }


    sem_t* mutex;
    sem_t* count;
    int shmid;

    // Creating Shared Memory Segment
    shmid = shmget(IPC_PRIVATE, sizeof(sem_t), (S_IRUSR|S_IWUSR));
    if (shmid == -1) {
        perror("Creation of Shared Memory FAILED\n");
        exit(EXIT_FAILURE);
    }

    // Ataching Memory Segment
    mutex = (sem_t*)shmat(shmid, NULL, 0);
    count = mutex + 1;
    //char* message = (char*)(count + 1);

    sem_init(mutex, 1/*shared across processes*/, 1);
    sem_init(count, 1, 0);

    pthread_t recThread;
    int res;

    res = pthread_create(&recThread, NULL, receive_Thread, (void*)"B");
    if (res != 0) {
        printf("Creation of thread failed\n");
        exit(EXIT_FAILURE);
    }

    pthread_t inpThread;
    int res2;

    res2 = pthread_create(&inpThread, NULL, input_Thread, (void*)"A");

    if (res2 != 0) {
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