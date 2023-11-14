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

typedef struct {
    char msg[BUFSIZE];
    int size;
}Message;

typedef struct{
    sem_t sem;
    int count; // Number of messsages
    Message message;
}SharedMemory;

SharedMemory *sharedMem;

void* input_Thread(void* item) {
    char* childID;
    childID = (char*)item;

    printf("We are in process %s", childID);
    struct timespec sleeptime;
    sleeptime.tv_sec = 0;
    sleeptime.tv_nsec = 100000000L;

    SharedMemory* sharedMem = malloc(sizeof(*sharedMem));

    while(1) {
        printf("Type any message to continue, and type #BYE# if you want to terminate the process\n");
        char input[MAX_SIZE_OF_MESSAGE];
        fgets(input, MAX_SIZE_OF_MESSAGE, stdin);

        sem_wait(&(sharedMem->sem));

        if (!strcmp(input, "#BYE#")) {
            sem_post(&(sharedMem->sem));
            break;
        }
        strncpy(sharedMem->message.msg, input, MAX_SIZE_OF_MESSAGE);
        sharedMem->message.size = strlen(input);
        sharedMem->count++;

        sem_post(&(sharedMem->sem));
    }

    free(sharedMem);
    pthread_exit(NULL);
}


void *receive_Thread(void *item) {
    char *childID;
    childID = (char *)item;

    while (1) {
        sem_wait(&sharedMem->count);
        sem_wait(&sharedMem->sem);

        if (!strcmp(sharedMem->message.msg, "#BYE#")) {
            sem_post(&sharedMem->sem);
            break;
        }

        // Process the received message (e.g., print or save to file)
        printf("Received message: %s", sharedMem->message.msg);

        sem_post(&sharedMem->sem);
    }

    pthread_exit(NULL);
}


int main(int argc, char* argv[]) {
    if (argc == 1) {
        printf("No arguments were given\n");
    }
    else {
        printf("argument given is: %s \n", argv[1]);
    }


    // Creating Shared Memory Segment
    int shmid = shmget(IPC_PRIVATE, sizeof(SharedMemory), (S_IRUSR|S_IWUSR));
    if (shmid == -1) {
        perror("Creation of Shared Memory FAILED\n");
        exit(EXIT_FAILURE);
    }

    // Ataching Memory Segment
    sharedMem = (SharedMemory *)shmat(shmid, NULL, 0);
    if ((void *)sharedMem == (void *)-1) {
        perror("Failed to Atach Memory Segment");
        exit(EXIT_FAILURE);
    }

    sem_init(&sharedMem->sem, 1, 1);
    sem_init(&sharedMem->count, 1, 0);

    char* proc = "A";
    int res1, res2;
    pthread_t inputThread, receiveThread;

    res1 = pthread_create(&inputThread, NULL, input_Thread, (void*)proc);

    if (res1 != 0) {
        printf("Thread Creation Failed\n");
        exit(EXIT_FAILURE);
    }

    // res2 = pthread_create(&receiveThread, NULL, receive_Thread, (void*)proc);

    return 0;
}