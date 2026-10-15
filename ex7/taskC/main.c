#define _GNU_SOURCE
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sched.h>
#include <pthread.h>
#include <wiringPi.h>

#define PININ1  8
#define PINOUT1 9
#define PININ2  7
#define PINOUT2 15
#define PININ3  16
#define PINOUT3 1

#define CPU_ID  0   // all response threads run on this core

#define NUM_DISTURBANCE 10

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

// Pin the calling thread to a single CPU core
int set_cpu(int cpu_id) {
    int num_cores = sysconf(_SC_NPROCESSORS_ONLN);
    if (cpu_id < 0 || cpu_id >= num_cores)
        return EINVAL;

    cpu_set_t cpu;
    CPU_ZERO(&cpu);
    CPU_SET(cpu_id, &cpu);
    return pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpu);
}

// Busy-wait on the test signal (active low) and answer as fast as possible
void *responseTask(void *args) {
    struct responseTaskArgs a = *(struct responseTaskArgs *)args;

    set_cpu(CPU_ID);
/*
    while (1) {
        while (digitalRead(a.pin_in) == HIGH) { }   // wait for test signal
        digitalWrite(a.pin_out, LOW);               // respond
        while (digitalRead(a.pin_in) == LOW) { }    // wait for BRTT to release
        digitalWrite(a.pin_out, HIGH);              // reset response
    }
*/
    while (1) {
    while (digitalRead(a.pin_in) == HIGH) { sched_yield(); }  // Yield for å la andre kjøre
    digitalWrite(a.pin_out, LOW);
    while (digitalRead(a.pin_in) == LOW)  { sched_yield(); }
    digitalWrite(a.pin_out, HIGH);
}
    return NULL;
}

void *disturbanceTask(void *args) {
    set_cpu(CPU_ID);
    while (1) {
        asm volatile("" ::: "memory");
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
    for (int i = 0; i < 3; i++) {
        pthread_create(&threads[i], NULL, responseTask, &args[i]);
    }

    pthread_t disturbance[NUM_DISTURBANCE];
    for (int i = 0; i < NUM_DISTURBANCE; i++) {
    pthread_create(&disturbance[i], NULL, disturbanceTask, NULL);
    }


    printf("Response threads running on CPU %d. Start the test on the BRTT.\n", CPU_ID);
    fflush(stdout);

    for (int i = 0; i < 3; i++) {
        pthread_join(threads[i], NULL);
    }
    return 0;
}