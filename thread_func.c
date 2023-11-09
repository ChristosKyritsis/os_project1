#include "main.c"

void* inputThread(void* item) {
    enum ProcessType processType = *((enum ProcessType*)item);

    while(1) {
        printf("Give any message to continue, and type #BYE# if you want to terminate the process\n");
        char input[MAX_SIZE_OF_MESSAGE];
        fgets(input, MAX_SIZE_OF_MESSAGE, stdin);

        sem_wait(&(sharedMemory->sem));

        if (!strcmp(input, "#BYE#")) {
            sem_post(&(sharedMemory->sem));
            break;
        }
        strncpy(sharedMemory->message.msg, input, MAX_SIZE_OF_MESSAGE);
        sharedMemory->message.size = strlen(input);
        sharedMemory->count++;

        sem_post(&(sharedMemory->sem));
    }
    pthread_exit(NULL);
}