#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <wiringPi.h>

#define PININ1  8
#define PINOUT1 9
#define PININ2  7
#define PINOUT2 15
#define PININ3  16
#define PINOUT3 1

struct responseTaskArgs {
    int pin_in;
    int pin_out;
};

void io_init(void) {
    wiringPiSetup();

    pinMode(PININ1, INPUT); pinMode(PINOUT1, OUTPUT);
    pinMode(PININ2, INPUT); pinMode(PINOUT2, OUTPUT);
    pinMode(PININ3, INPUT); pinMode(PINOUT3, OUTPUT);

    digitalWrite(PINOUT1, HIGH);
    digitalWrite(PINOUT2, HIGH);
    digitalWrite(PINOUT3, HIGH);
}

// Busy-wait
void *responseTask(void *args) {
    struct responseTaskArgs a = *(struct responseTaskArgs *)args;

    while (1) {
        while (digitalRead(a.pin_in) == HIGH) { }   // wait 
        digitalWrite(a.pin_out, LOW);               // respond
        while (digitalRead(a.pin_in) == LOW) { }    // wait
        digitalWrite(a.pin_out, HIGH);              // reset
    }
    return NULL;
}

int main(void) {
    io_init();

    struct responseTaskArgs args[3] = {
        { PININ1, PINOUT1 },   // test A
        { PININ2, PINOUT2 },   // test B
        { PININ3, PINOUT3 },   // test C
    };

    pthread_t threads[3];
    for (int i = 0; i<3; i++){
        pthread_create(&threads[i], NULL, responseTask, &args[i]);
    };


    for (int i = 0; i<3; i++) {
        pthread_join(threads[i], NULL);
    }
    return 0;
}