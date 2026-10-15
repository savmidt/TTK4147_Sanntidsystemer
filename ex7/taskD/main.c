#define _GNU_SOURCE
#include <stdio.h>
#include <errno.h>
#include <time.h>
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

#define CPU_ID  0               // all threads run on this core

#define PERIOD_NS 2000000L      // polling period: 2 ms

#define USE_DISTURBANCE 1       // 0 = without disturbance, 1 = with
#define NUM_DISTURBANCE 2

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

// Pinner til CPU
int set_cpu(int cpu_id) {
    int num_cores = sysconf(_SC_NPROCESSORS_ONLN);
    if (cpu_id < 0 || cpu_id >= num_cores)
        return EINVAL;

    cpu_set_t cpu;
    CPU_ZERO(&cpu);
    CPU_SET(cpu_id, &cpu);
    return pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpu);
}

// Add nanoseconds
void timespec_add_ns(struct timespec *t, long ns) {
    t->tv_nsec += ns;
    while (t->tv_nsec >= 1000000000L) {
        t->tv_nsec -= 1000000000L;
        t->tv_sec++;
    }
}

// Periodic polling
void *responseTask(void *args) {
    struct responseTaskArgs a = *(struct responseTaskArgs *)args;

    int err = set_cpu(CPU_ID);

    struct timespec next;
    clock_gettime(CLOCK_MONOTONIC, &next);

    while (1) {
        if (digitalRead(a.pin_in) == LOW) {              // test signal active
            digitalWrite(a.pin_out, LOW);                // respond
            while (digitalRead(a.pin_in) == LOW) { }     // BRTT releases right after the response
            digitalWrite(a.pin_out, HIGH);               // reset response
        }

        // Sleep until the next absolute period start (no drift)
        timespec_add_ns(&next, PERIOD_NS);
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, NULL);
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
        if (pthread_create(&threads[i], NULL, responseTask, &args[i]) != 0) {
            perror("pthread_create");
            return 1;
        }
    }

#if USE_DISTURBANCE
    pthread_t disturbance[NUM_DISTURBANCE];
    for (int i = 0; i < NUM_DISTURBANCE; i++) {
        if (pthread_create(&disturbance[i], NULL, disturbanceTask, NULL) != 0) {
            perror("pthread_create disturbance");
            return 1;
        }
    }
    printf("Periodic polling (%ld us) on CPU %d with %d disturbance threads.\n",
           PERIOD_NS / 1000, CPU_ID, NUM_DISTURBANCE);
#else
    printf("Periodic polling (%ld us) on CPU %d without disturbance.\n",
           PERIOD_NS / 1000, CPU_ID);
#endif
    printf("Start the test on the BRTT.\n");
    fflush(stdout);

    for (int i = 0; i < 3; i++) {
        pthread_join(threads[i], NULL);
    }
    return 0;
}