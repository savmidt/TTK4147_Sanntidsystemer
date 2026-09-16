#include "file_handling.h"
#include "schedulers.h"
#include "scheduling.h"

struct Task {
    enum taskState state;
    int ID;
    int arrivalTime;
    int startTime;          // Some time unit
    int totalRuntime;       // Some time unit
    int currentRuntime;     // Some time unit
};
    
int main(){

    return 0;
}