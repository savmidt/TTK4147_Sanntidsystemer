#include <sys/time.h>
#include <time.h>
#include <stdlib.h>
#include <sys/times.h>
#include <stdio.h>
#include <unistd.h>

uint64_t rdtsc(void){
    uint64_t val;
    asm volatile("isb; mrs %0, cntvct_el0; isb; " : "=r"(val) :: "memory"); 
    // You can check the current CPU frequency with $sudo dmesg | grep MHz
    return val;
}

void func(){

    // ------------------rdtsc--------------------
    uint64_t start, end;
    start = rdtsc();
    for(int i = 0; i < 10*1000*1000; i++){
        // read timer
        end = rdtsc();
    }
    long latency = (end - start) / 10000000;

    uint64_t freq; asm volatile("mrs %0, cntfrq_el0" : "=r"(freq));
    int ns_max = 50;
    int histogram1[ns_max];
    memset(histogram1, 0, sizeof(int)*ns_max);
    for(int i = 0; i < 10*1000*1000; i++){
        uint64_t t1 = rdtsc();
        uint64_t t2 = rdtsc();

        int ns = (int)((t2 - t1) * 1000000000ULL / freq);
        if (ns >= 0 && ns < ns_max) histogram1[ns]++;
    }
    long resolution = 
    
    // ------------------clock_gettime--------------------
    clock_t time;
    clock_t start_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    for(int i = 0; i < 10*1000*1000; i++){
        // read timer
        clock_gettime(CLOCK_MONOTONIC, &time);
    }
    long latency = (long)((time.tv_sec - start_time.tv_sec)*1000000000L + (time.tv_nsec - start_time.tv_nsec)) / N;

    int histogram2[ns_max];
    memset(histogram2, 0, sizeof(int)*ns_max);
        for (int i = 0; i < N; i++) {
            struct timespec t1, t2;
            clock_gettime(CLOCK_MONOTONIC, &t1);
            clock_gettime(CLOCK_MONOTONIC, &t2);
            int ns = (int)((t2.tv_sec - t1.tv_sec)*1000000000L + (t2.tv_nsec - t1.tv_nsec));
            if (ns >= 0 && ns < ns_max) histogram2[ns]++;
        }

    // ------------------times--------------------
    int histogram3[ns_max];
    struct tms start_times, end_times;
    clock_t start_real = times(&start);
    clock_t end_real = times(&end);
    for(int i = 0; i < 10*1000*1000; i++){
        // read timer
        end_real = times(&end);
    }
    int latency = (int) start_time-time
}

int main(){
    func();
}