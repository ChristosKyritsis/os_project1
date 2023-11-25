#include "inc.h"

#define MAX_SIZE_OF_MESSAGE 15
#define SEM_PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP) // code from lab might delete later


void* input_thread_B(void* arg) {
    SharedData* data = (SharedData*)arg;
    char msg[BUFFSIZE];

    while (true) {
        printf("Process A sent: %s\n", data->messageA);
        printf("Please enter any message for Process A or type #BYE# to terminate the process: ");

        fgets(msg, MAX_SIZE_OF_MESSAGE, stdin);
        memcpy(&data->messageB, msg, strlen(msg));
        sem_post(&data->semB);

        if (strcmp(data->messageB, "#BYE#\n") == 0) {
            data->finished = true;
            sem_post(&data->semB);
            break;
        }
    }

    pthread_exit(NULL);
}


void* receive_thread_B(void* arg) {
    SharedData* data = (SharedData*)arg;
    char msg[BUFFSIZE];
    struct timeval begin, end;
    double totalTime = 0.0;

    while (true) {
        sem_wait(&data->semA);

        if (data->finished == true)
            break;

        printf("Process A sent: %s\n", data->messageA);
        
        gettimeofday(&end, NULL);
        totalTime = end.tv_sec - begin.tv_sec;

        data->countB++;
        data->numOfPiecesB +=strlen(data->messageA);
        data->waitingTimeB += totalTime;

        printf("Please enter any message for Process A or type #BYE# to terminate the process: ");
        gettimeofday(&begin, NULL);
        fgets(msg, MAX_SIZE_OF_MESSAGE, stdin);
        memcpy(&data->messageB, msg, strlen(msg));
        sem_post(&data->semB);

        if (strcmp(data->messageB, "#BYE#\n") == 0) {
            data->finished = true;
            sem_post(&data->semB);
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

    res1 = pthread_create(&recThread, NULL, receive_thread_B, (void*)data);
    if (res1 != 0) {
        printf("Creation of thread failed\n");
        exit(EXIT_FAILURE);
    }

  
    res2 = pthread_create(&inpThread, NULL, input_thread_B, (void*)data);

    if (res2 != 0) {
        printf("Creation of thread failed\n");
        exit(EXIT_FAILURE);
    }

    pthread_join(inpThread, NULL);
    pthread_join(recThread, NULL);

    free_data(data);
    munmap(data, sizeof(SharedData));
    //unlink(argv[1]);

    return 0;
}