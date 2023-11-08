#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int main(int argc, char** argv) {
    if (argc == 1) {
        printf("No arguments were given\n");
    }
    else {
        printf(argv[1], "\n");
    }
    return 0;
}