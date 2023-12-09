#define _POSIX_C_SOURCE 200112L         // in order to use ftruncate

#include "inc.h"


void* input_thread(void* arg) {
    SharedData* data = (SharedData*)arg;
    
    while (true) {
        fgets(data->messageA, BUFFSIZE, stdin);

        sem_post(&data->semA);

        if (strcmp(data->messageA, "#BYE#\n") == 0) {
            sem_post(&data->semA);
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
        gettimeofday(&begin, NULL);

        if (strcmp(data->messageB, "#BYE#\n") == 0) 
            break;
        

        int length = strlen(data->messageB);

        // MAX_SIZE_OF_MESSAGE + 1 because there is a "\n" character in the end
        if (length <= MAX_SIZE_OF_MESSAGE + 1) {
            printf("Process B sent: %s\n", data->messageB);
        }
        else {
            // Takes the oversized message and types it in messages of 15
            // variable counter is used to keep track of how many messages the initial message is split into
            printf("The message that Process B sent was over 15 characters so it will be printed in strings of 15 at most\n\n");
            int counter = 1;
            printf("Process B sent: \n");
            printf("String number %d \t ", counter);
            for (int j = 0; j < MAX_SIZE_OF_MESSAGE && j < length; ++j) {
                printf("%c", data->messageB[j]);
            }
            counter++;
            printf("\n");

            for (int i = MAX_SIZE_OF_MESSAGE; i < length; i += 15) {
                printf("String number %d \t ", counter);
                for (int k = 0; k < MAX_SIZE_OF_MESSAGE && i + k < length; ++k) {
                    printf("%c", data->messageB[i+k]);
                }
                counter++;
                printf("\n");
            }

        }
        
        gettimeofday(&end, NULL);
        // Dividing by 1000000.0 in order to turn the microseconds to seconds
        totalTime = (end.tv_sec - begin.tv_sec) + (end.tv_usec - begin.tv_usec)/1000000.0;

        // Updating process stats
        data->countB++;
        data->numOfPiecesA += strlen(data->messageB) - 1;   // -1 because of the "\n" character
        data->waitingTimeA += totalTime;
    }
    pthread_exit(NULL);
}


int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("No arguments given on command line.\nPlease run the program again but this time use an argument\n\n");
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
    
    // truncating the file to the length of SharedData
    int trunc;
    trunc = ftruncate(fd, sizeof(SharedData));
    if (trunc == -1) {
        printf("Failed to truncate\n");
        exit(EXIT_FAILURE);
    }

    SharedData* data = (SharedData*)mmap(NULL, sizeof(SharedData), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (data == MAP_FAILED) {
        printf("Mapping failed\n");
        exit(EXIT_FAILURE);
    }

    close(fd);

    // Process A is the one in charge of initializing the data
    initialize_data(data);

    printf("-----THIS IS PROCESS A-----\n\n");

    // Creating and joining threads
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

    if (pthread_join(inpThread, NULL) != 0) {
        printf("Error on pthread_join\n");
        exit(EXIT_FAILURE);
    }
    if (pthread_join(recThread, NULL) != 0) {
        printf("Error on pthread_join\n");
        exit(EXIT_FAILURE);
    }

    // Process A is the one in charge of freeing the data
    free_data(data);

    // unmaping
    if (munmap(data, sizeof(SharedData)) == -1) {
        printf("Failed to unmap\n");
        exit(EXIT_FAILURE);
    }

    // removing shared memory object
    if (shm_unlink(shmpath) == -1) {
        printf("Failed to remove shared memory object\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}