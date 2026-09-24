#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_PHILOSOPHERS 5
#define NUM_MEALS        1

/*
 * 0 = naive version (will deadlock)
 * 1 = fixed version (no deadlock)
 * Set from the command line in main().
 */
int fix_deadlock = 0;

/* One mutex per fork */
pthread_mutex_t forks[NUM_PHILOSOPHERS];

void *philosopher(void *arg)
{
    int id = *(int *)arg;

    /* Each philosopher sits between two forks */
    int left  = id;
    int right = (id + 1) % NUM_PHILOSOPHERS;

    /* Naive: everyone picks up the left fork first, then the right */
    int first  = left;
    int second = right;

    /*
     * Fix: always pick up the lower-numbered fork first.
     * This only changes anything for the last philosopher, who now
     * reaches for fork 0 before fork 4. That breaks the circle, so
     * everyone can no longer end up waiting on each other.
     */
    if (fix_deadlock && first > second) {
        first  = right;
        second = left;
    }

    for (int meal = 1; meal <= NUM_MEALS; meal++) {
        printf("Philosopher %d is thinking\n", id);
        usleep(100000); /* think for 0.1 s */

        pthread_mutex_lock(&forks[first]);
        printf("Philosopher %d picked up fork %d\n", id, first);

        /* Small pause so that everyone grabs their first fork
           at the same time. This makes the deadlock easy to see. */
        usleep(100000);

        pthread_mutex_lock(&forks[second]);
        printf("Philosopher %d picked up fork %d\n", id, second);

        printf("Philosopher %d is eating (meal %d)\n", id, meal);
        usleep(100000); /* eat for 0.1 s */

        /* Put the forks down (same thread that locked them) */
        pthread_mutex_unlock(&forks[second]);
        pthread_mutex_unlock(&forks[first]);
        printf("Philosopher %d put down forks %d and %d\n", id, first, second);
    }

    printf("Philosopher %d is done\n", id);
    return NULL;
}

int main(int argc, char *argv[])
{
    pthread_t threads[NUM_PHILOSOPHERS];
    int ids[NUM_PHILOSOPHERS];

    /* Read the command line argument:
       ./philosophers        -> naive version
       ./philosophers fix    -> fixed version */
    if (argc > 1 && strcmp(argv[1], "fix") == 0) {
        fix_deadlock = 1;
        printf("Running FIXED version\n");
    } else {
        printf("Running NAIVE version (expect a deadlock)\n");
    }

    /* Create the forks */
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_mutex_init(&forks[i], NULL);
    }

    /* Start the philosophers */
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, philosopher, &ids[i]);
    }

    /* Wait for all philosophers to finish */
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_join(threads[i], NULL);
    }

    /* Clean up */
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_mutex_destroy(&forks[i]);
    }

    printf("All philosophers have finished eating\n");
    return 0;
}