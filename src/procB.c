#include "inc.h"

#define MAX_SIZE_OF_MESSAGE 15
#define SEM_PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP) // code from lab might delete later


void* input_thread_B(void* arg) {
    SharedData* data = (SharedData*)arg;

    while (true) {
        fgets(data->messageB, BUFFSIZE, stdin);

        sem_post(&data->semB);

        if (strcmp(data->messageB, "#BYE#\n") == 0) {
            data->finished = true;
            print_data(data);
            break;
        }
    }
    pthread_exit(NULL);
}


void* receive_thread_B(void* arg) {
    SharedData* data = (SharedData*)arg;
    struct timeval begin, end;
    double totalTime = 0.0;

    while (true) {
        sem_wait(&data->semA);

        if (data->finished == true)
            break;

        int length = strlen(data->messageA);

        if (length <= MAX_SIZE_OF_MESSAGE) {
            printf("Process A sent: %s\n", data->messageA);
        }
        else {
            printf("The message that Process A sent was over 15 characters so it will be printed in string of 15\n");
            printf("Process A sent: ");
            for (int j = 0; j < MAX_SIZE_OF_MESSAGE && j < length; ++j) {
                printf("%c", data->messageA[j]);
            }
            printf("\n");

            for (int i = MAX_SIZE_OF_MESSAGE; i < length; i += 15) {
                printf("Process A sent: ");
                for (int k = 0; k < MAX_SIZE_OF_MESSAGE && i + k < length; ++k) {
                    printf("%c", data->messageA[i+k]);
                }
                printf("\n");
            }

        }

        gettimeofday(&end, NULL);
        totalTime = (end.tv_sec - begin.tv_sec) + (end.tv_usec - begin.tv_usec)/1000000.0;

        data->countA++;
        data->numOfPiecesB +=strlen(data->messageA) -1;
        data->waitingTimeB += totalTime;

        gettimeofday(&begin, NULL);

        if (strcmp(data->messageA, "#BYE#\n") == 0) {
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

    close(fd);

    printf("-----THIS IS PROCESS B-----\n\n");

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

    munmap(data, sizeof(SharedData));

    return 0;
}