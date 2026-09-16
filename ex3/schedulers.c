#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>
#include "scheduling.h"
#include "schedulers.h"

void set_task_state(struct Task *task, enum taskState taskNewState)
{
    pthread_mutex_lock(&taskStateMutex);
    task->state = taskNewState;
    pthread_mutex_unlock(&taskStateMutex);
}

void wait_for_rescheduling(int quantum, struct Task *task)
{
    int startTime;
    int waitTime;

    pthread_mutex_lock(&timeMutex);
    startTime = globalTime;
    pthread_mutex_unlock(&timeMutex);

    do
    {
        pthread_mutex_lock(&timeMutex);
        pthread_cond_wait(&timeCond, &timeMutex);
        waitTime = globalTime - startTime;
        pthread_mutex_unlock(&timeMutex);
    } while (task->state != finished && waitTime < quantum);

    usleep(timeUnitUs / 100);
}

void round_robin(struct Task **tasks, int taskCount, int timeout, int quantum)
{
    int taskIndex = 0;

    do
    {
        // Skip finished tasks or those that have not arrived yet
        if (tasks[taskIndex]->state == finished || tasks[taskIndex]->arrivalTime > globalTime)
        {
            taskIndex = (taskIndex + 1) % taskCount;
            continue;
        }

        // Set the task state to running
        if (tasks[taskIndex]->startTime == -1)
            tasks[taskIndex]->startTime = globalTime;
        set_task_state(tasks[taskIndex], running);

        // Wait for the quantum interval
        wait_for_rescheduling(quantum, tasks[taskIndex]);

        //  Check if the task is finished
        if (tasks[taskIndex]->state == finished)
        {
        }
        else
        {
            set_task_state(tasks[taskIndex], preempted);
        }

        // Find the next task to run
        taskIndex = (taskIndex + 1) % taskCount;

    } while (globalTime < timeout);
}

// Implement your schedulers here!
void first_come_first_served(struct Task **tasks, int taskCount, int timeout)
{
    do
    {
        // Pick the arrived, unfinished task with the earliest arrival time
        int next = -1;
        int earliestArrival = timeout + 1;
        for (int i = 0; i < taskCount; i++)
        {
            if (tasks[i]->state != finished && tasks[i]->arrivalTime <= globalTime)
            {
                if (tasks[i]->arrivalTime < earliestArrival ||
                    (tasks[i]->arrivalTime == earliestArrival && (next == -1 || i < next)))
                {
                    earliestArrival = tasks[i]->arrivalTime;
                    next = i;
                }
            }
        }

        if (next == -1)
        {
            // No task ready yet — wait for the next time tick
            pthread_mutex_lock(&timeMutex);
            pthread_cond_wait(&timeCond, &timeMutex);
            pthread_mutex_unlock(&timeMutex);
            continue;
        }

        if (tasks[next]->startTime == -1)
            tasks[next]->startTime = globalTime;
        set_task_state(tasks[next], running);

        // Non-preemptive: let it run to completion
        wait_for_rescheduling(timeout, tasks[next]);

    } while (globalTime < timeout);
}

void shortest_process_next(struct Task **tasks, int taskCount, int timeout)
{
    do
    {
        // Pick the arrived, unfinished task with the shortest total runtime
        int next = -1;
        int shortestRuntime = __INT_MAX__;
        for (int i = 0; i < taskCount; i++)
        {
            if (tasks[i]->state != finished && tasks[i]->arrivalTime <= globalTime)
            {
                if (tasks[i]->totalRuntime < shortestRuntime ||
                    (tasks[i]->totalRuntime == shortestRuntime && (next == -1 || tasks[i]->arrivalTime < tasks[next]->arrivalTime)))
                {
                    shortestRuntime = tasks[i]->totalRuntime;
                    next = i;
                }
            }
        }

        if (next == -1)
        {
            pthread_mutex_lock(&timeMutex);
            pthread_cond_wait(&timeCond, &timeMutex);
            pthread_mutex_unlock(&timeMutex);
            continue;
        }

        if (tasks[next]->startTime == -1)
            tasks[next]->startTime = globalTime;
        set_task_state(tasks[next], running);

        // Non-preemptive
        wait_for_rescheduling(timeout, tasks[next]);

    } while (globalTime < timeout);
}

void highest_response_ratio_next(struct Task **tasks, int taskCount, int timeout)
{
    do
    {
        // Pick the arrived, unfinished task with the highest response ratio
        // RR = (waitingTime + totalRuntime) / totalRuntime
        int next = -1;
        double highestRR = -1.0;
        for (int i = 0; i < taskCount; i++)
        {
            if (tasks[i]->state != finished && tasks[i]->arrivalTime <= globalTime)
            {
                int waitingTime = globalTime - tasks[i]->arrivalTime;
                double rr = (double)(waitingTime + tasks[i]->totalRuntime) / tasks[i]->totalRuntime;
                if (rr > highestRR ||
                    (rr == highestRR && (next == -1 || tasks[i]->arrivalTime < tasks[next]->arrivalTime)))
                {
                    highestRR = rr;
                    next = i;
                }
            }
        }

        if (next == -1)
        {
            pthread_mutex_lock(&timeMutex);
            pthread_cond_wait(&timeCond, &timeMutex);
            pthread_mutex_unlock(&timeMutex);
            continue;
        }

        if (tasks[next]->startTime == -1)
            tasks[next]->startTime = globalTime;
        set_task_state(tasks[next], running);

        // Non-preemptive
        wait_for_rescheduling(timeout, tasks[next]);

    } while (globalTime < timeout);
}

void shortest_remaining_time(struct Task **tasks, int taskCount, int timeout, int quantum)
{
    do
    {
        // Pick the arrived, unfinished task with the shortest remaining time
        int next = -1;
        int shortestRemaining = __INT_MAX__;
        for (int i = 0; i < taskCount; i++)
        {
            if (tasks[i]->state != finished && tasks[i]->arrivalTime <= globalTime)
            {
                int remaining = tasks[i]->totalRuntime - tasks[i]->currentRuntime;
                if (remaining < shortestRemaining ||
                    (remaining == shortestRemaining && (next == -1 || tasks[i]->arrivalTime < tasks[next]->arrivalTime)))
                {
                    shortestRemaining = remaining;
                    next = i;
                }
            }
        }

        if (next == -1)
        {
            pthread_mutex_lock(&timeMutex);
            pthread_cond_wait(&timeCond, &timeMutex);
            pthread_mutex_unlock(&timeMutex);
            continue;
        }

        if (tasks[next]->startTime == -1)
            tasks[next]->startTime = globalTime;
        set_task_state(tasks[next], running);

        // Preemptive: run for one quantum then re-evaluate
        wait_for_rescheduling(quantum, tasks[next]);

        if (tasks[next]->state != finished)
            set_task_state(tasks[next], preempted);

    } while (globalTime < timeout);
}

void feedback(struct Task **tasks, int taskCount, int timeout, int quantum)
{
    // Priority per task — starts at 0 (highest), increases on preemption
    int *priority = calloc(taskCount, sizeof(int));

    int taskIndex = 0;

    do
    {
        // Find the lowest priority value (highest priority) among ready tasks
        int bestPriority = __INT_MAX__;
        for (int i = 0; i < taskCount; i++)
        {
            if (tasks[i]->state != finished && tasks[i]->arrivalTime <= globalTime)
            {
                if (priority[i] < bestPriority)
                    bestPriority = priority[i];
            }
        }

        if (bestPriority == __INT_MAX__)
        {
            // No task ready
            pthread_mutex_lock(&timeMutex);
            pthread_cond_wait(&timeCond, &timeMutex);
            pthread_mutex_unlock(&timeMutex);
            continue;
        }

        // Round-robin among tasks at the best priority level
        // Find the next eligible task starting from taskIndex
        int found = -1;
        for (int i = 0; i < taskCount; i++)
        {
            int idx = (taskIndex + i) % taskCount;
            if (tasks[idx]->state != finished && tasks[idx]->arrivalTime <= globalTime && priority[idx] == bestPriority)
            {
                found = idx;
                break;
            }
        }

        if (found == -1)
        {
            taskIndex = (taskIndex + 1) % taskCount;
            continue;
        }

        if (tasks[found]->startTime == -1)
            tasks[found]->startTime = globalTime;
        set_task_state(tasks[found], running);

        wait_for_rescheduling(quantum, tasks[found]);

        if (tasks[found]->state != finished)
        {
            set_task_state(tasks[found], preempted);
            priority[found]++;  // Demote to lower priority queue
        }

        taskIndex = (found + 1) % taskCount;

    } while (globalTime < timeout);

    free(priority);
}