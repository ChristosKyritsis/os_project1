#include "inc.h"


void* input_thread_B(void* arg) {
    SharedData* data = (SharedData*)arg;

    while (true) {
        fgets(data->messageB, BUFFSIZE, stdin);

        sem_post(&data->semB);

        if (strcmp(data->messageB, "#BYE#\n") == 0) {
            sem_post(&data->semB);
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
        // Waiting for Process A to type a message
        sem_wait(&data->semA);
        gettimeofday(&begin, NULL);

        if (strcmp(data->messageA, "#BYE#\n") == 0) 
            break;
        
        int length = strlen(data->messageA);

        // MAX_SIZE_OF_MESSAGE + 1 because there is a "\n" character in the end
        if (length <= MAX_SIZE_OF_MESSAGE + 1) {
            printf("Process A sent: %s\n", data->messageA);
        }
        else {
            // Takes the oversized message and types it in messages of 15
            // variable counter is used to keep track of how many messages the initial message is split into 
            int counter = 1;
            printf("The message that Process A sent was over 15 characters so it will be printed in strings of 15 at most\n\n");
            printf("Process A sent: \n");
            printf("String number %d \t ", counter);
            for (int j = 0; j < MAX_SIZE_OF_MESSAGE && j < length; ++j) {
                printf("%c", data->messageA[j]);
            }
            counter++;
            printf("\n");

            for (int i = MAX_SIZE_OF_MESSAGE; i < length; i += 15) {
                printf("String number %d \t ", counter);
                for (int k = 0; k < MAX_SIZE_OF_MESSAGE && i + k < length; ++k) {
                    printf("%c", data->messageA[i+k]);
                }
                counter++;
                printf("\n");
            }

        }

        gettimeofday(&end, NULL);
        // 1000000.0 the amount required to turn microseconds to seconds
        totalTime = (end.tv_sec - begin.tv_sec) + (end.tv_usec - begin.tv_usec)/1000000.0;

        // updating the info of the process
        data->countA++;
        data->numOfPiecesB +=strlen(data->messageA) -1;     // -1 because there is a "\n" character at the end
        data->waitingTimeB += totalTime;
    }
    pthread_exit(NULL);
}


int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("No arguments given on command line.\nPlease run the program again but this time use an argument\n\n");
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
        printf("Mapping failed\n");
        exit(EXIT_FAILURE);
    }

    close(fd);

    printf("-----THIS IS PROCESS B-----\n\n");

    // Creating and joining threads
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

    if (pthread_join(inpThread, NULL) != 0) {
        printf("Error on pthread_join\n");
        exit(EXIT_FAILURE);
    }

    if (pthread_join(recThread, NULL) != 0) {
        printf("Error on pthread_join\n");
        exit(EXIT_FAILURE);
    }

    // unmaping
    if (munmap(data, sizeof(SharedData)) == -1) {
        printf("Failed to unmap\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}