#define _POSIX_C_SOURCE 200112L         // in order to use ftruncate

#include "inc.h"

//#define _POSIX_C_SOURCE 200809L

#define MAX_SIZE_OF_MESSAGE 15

#define SEM_PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP) // code from lab


void* input_thread(void* arg) {
    SharedData* data = (SharedData*)arg;
    
    while (true) {
        fgets(data->messageA, BUFFSIZE, stdin);

        sem_post(&data->semA);

        if (strcmp(data->messageA, "#BYE#\n") == 0) {
            data->finished = true;
            print_data(data);
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
        sem_wait(&data->semB);

        if (data->finished == true) 
            break;      

        int length = strlen(data->messageB);

        if (length <= MAX_SIZE_OF_MESSAGE) {
            printf("Process B sent: %s\n", data->messageB);
        }
        else {
            printf("The message that Process B sent was over 15 characters so it will be printed in string of 15\n");
            printf("Process B sent: ");
            for (int j = 0; j < MAX_SIZE_OF_MESSAGE && j < length; ++j) {
                printf("%c", data->messageB[j]);
            }
            printf("\n");

            for (int i = MAX_SIZE_OF_MESSAGE; i < length; i += 15) {
                printf("Process B sent: ");
                for (int k = 0; k < MAX_SIZE_OF_MESSAGE && i + k < length; ++k) {
                    printf("%c", data->messageB[i+k]);
                }
                printf("\n");
            }

        }
        
        gettimeofday(&end, NULL);
        totalTime = (end.tv_sec - begin.tv_sec) + (end.tv_usec - begin.tv_usec)/1000000.0;

        data->countB++;
        data->numOfPiecesA += strlen(data->messageB) - 1;   // -1 because of the "\n" character
        data->waitingTimeA += totalTime;

        gettimeofday(&begin, NULL);

        if (strcmp(data->messageB, "#BYE#\n") == 0) {
            data->finished = true;
            print_data(data);
            break;
        }
    }
    pthread_exit(NULL);
}


int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("No arguments given on command line\n");
        exit(EXIT_FAILURE);
    }
    
    // Creation of shared memory
    char* shmpath = argv[1];
    int fd;
    fd = shm_open(shmpath, O_CREAT | O_EXCL | O_RDWR, 0666);
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

    close(fd);

    initialize_data(data);

    printf("-----THIS IS PROCESS A-----\n\n");

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
    shm_unlink(shmpath);

    return 0;
}