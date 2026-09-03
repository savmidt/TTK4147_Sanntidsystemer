#include "times.h"

int main(int argc, char *argv[]){
    char *endptr;
    long input = strtol(argv[1], &endptr, 10);
    if((int) input == 0){
        sleep(1);
    }
    else if((int) input == 1){
        struct timespec t = {.tv_sec = 1, .tv_nsec = 0};
        busy_wait(t);
    }
    else if((int) input == 2){
        double t = 1.0;
        busy_wait_alt(t);
    }
    return 0;
}