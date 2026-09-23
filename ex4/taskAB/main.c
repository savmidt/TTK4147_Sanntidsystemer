#include <pthread.h>
#include <stdio.h>

long global = 0;
pthread_barrier_t barrier;

void* fn(void* args){
    long local = 0;

    pthread_barrier_wait(&barrier);

    for(int i = 0; i < 1000000; i++){
        global++;
        local++;
    }

    printf("local: %ld, global: %ld\n", local, global);
    return NULL;
}

int main(){
    pthread_t thread1, thread2;

    pthread_barrier_init(&barrier, NULL, 2);

    pthread_create(&thread1, NULL, fn, NULL);
    pthread_create(&thread2, NULL, fn, NULL);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    pthread_barrier_destroy(&barrier);
    return 0;
}