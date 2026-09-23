#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>

long global = 0;
pthread_barrier_t barrier;
sem_t sem;

void* fn(void* args){
    long local = 0;

    pthread_barrier_wait(&barrier);

    for(int i = 0; i < 1000000; i++){
        sem_wait(&sem);
        global++;
        sem_post(&sem);
        local++;
    }

    sem_wait(&sem);
    printf("local: %ld, global: %ld\n", local, global);
    sem_post(&sem);
    return NULL;
}

int main(){
    pthread_t thread1, thread2;

    pthread_barrier_init(&barrier, NULL, 2);
    sem_init(&sem, 0, 1);

    pthread_create(&thread1, NULL, fn, NULL);
    pthread_create(&thread2, NULL, fn, NULL);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    sem_destroy(&sem);
    pthread_barrier_destroy(&barrier);
    return 0;
}