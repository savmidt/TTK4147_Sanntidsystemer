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
    uint64_t freq; asm volatile("mrs %0, cntfrq_el0" : "=r"(freq));
    long latency1 = (long)((end - start) * 1000000000ULL / freq) / N;

    int ns_max = 50;
    int histogram1[ns_max];
    memset(histogram1, 0, sizeof(int)*ns_max);
    uint64_t min_ticks = 0;
    for(int i = 0; i < N; i++){
        uint64_t t1 = rdtsc();
        uint64_t t2 = rdtsc();

        if (t2 - t1 > 0 && (min_ticks == 0 || t2 - t1 < min_ticks)) min_ticks = t2 - t1;

        int ns = (int)((t2 - t1) * 1000000000ULL / freq);
        if (ns >= 0 && ns < ns_max) histogram1[ns]++;
    }
    long resolution1 = (long)(min_ticks * 1000000000ULL / freq);
    
    // ------------------clock_gettime--------------------
    struct timespec time;
    struct timespec start_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    for(int i = 0; i < N; i++){
        // read timer
        clock_gettime(CLOCK_MONOTONIC, &time);
    }
    long latency2 = (long)((time.tv_sec - start_time.tv_sec)*1000000000L + (time.tv_nsec - start_time.tv_nsec)) / N;

    int histogram2[ns_max];
    memset(histogram2, 0, sizeof(int)*ns_max);
    long min_ns = 0;
    for (int i = 0; i < N; i++) {
        struct timespec t1, t2;
        clock_gettime(CLOCK_MONOTONIC, &t1);
        clock_gettime(CLOCK_MONOTONIC, &t2);
        int ns = (int)((t2.tv_sec - t1.tv_sec)*1000000000L + (t2.tv_nsec - t1.tv_nsec));
        if (ns > 0 && (min_ns == 0 || ns < min_ns)) min_ns = ns;
        if (ns >= 0 && ns < ns_max) histogram2[ns]++;
    }
    long resolution2 = min_ns;
    // ------------------times--------------------
    long tick_ns = 1000000000L / sysconf(_SC_CLK_TCK);   // ns per tick, typisk 10 ms

    int histogram3[ns_max];
    memset(histogram3, 0, sizeof(int)*ns_max);
    struct tms start_times, end_times;
    clock_t start_real = times(&start_times);
    clock_t end_real = times(&end_times);
    for(int i = 0; i < N; i++){
        // read timer
        end_real = times(&end_times);
    }
    long latency3 = (end_real - start_real) * tick_ns / N;

    for (int i = 0; i < N; i++) {
        clock_t t1 = times(&start_times);
        clock_t t2 = times(&end_times);
        long ns = (long)(t2 - t1) * tick_ns;
        if (ns >= 0 && ns < ns_max) histogram3[ns]++;
    }
    long resolution3 = tick_ns;


    // ------------------task C: sched_yield--------------------
    int ns_max_c = 2000;                 // mye større enn 50, syscall koster ~1 us
    int histogram4[ns_max_c];
    memset(histogram4, 0, sizeof(int)*ns_max_c);
    for (int i = 0; i < N; i++) {
        struct timespec t1, t2;
        clock_gettime(CLOCK_MONOTONIC, &t1);
        sched_yield();
        clock_gettime(CLOCK_MONOTONIC, &t2);
        int ns = (int)((t2.tv_sec - t1.tv_sec)*1000000000L + (t2.tv_nsec - t1.tv_nsec));
        if (ns >= 0 && ns < ns_max_c) histogram4[ns]++;
    }


    // print out for plot:
    // change histogram to 1, 2, 3(Task B) or 4(Task C)
    for(int i = 0; i < ns_max; i++){
        printf("%d\n", histogram1[i]);
    }
}

int main(){
    func();
}