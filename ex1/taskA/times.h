#include <sys/time.h>
#include <time.h>
#include <stdlib.h>
#include <sys/times.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>

struct timespec timespec_normalized(time_t sec, long nsec);
struct timespec timespec_sub(struct timespec lhs, struct timespec rhs);
struct timespec timespec_add(struct timespec lhs, struct timespec rhs);
int timespec_cmp(struct timespec lhs, struct timespec rhs);

void busy_wait(struct timespec t);
void busy_wait_alt(double sec_wait);