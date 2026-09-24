# Excercise 4

### Task A
```bash
student@raspberrypi:~/group21/ex4/taskAB $ ./taskA
local: 1000000, global: 941015
local: 1000000, global: 1005209
student@raspberrypi:~/group21/ex4/taskAB $ ./taskA
local: 1000000, global: 1002645
local: 1000000, global: 1014329
student@raspberrypi:~/group21/ex4/taskAB $ ./taskA
local: 1000000, global: 934309
local: 1000000, global: 1001545
student@raspberrypi:~/group21/ex4/taskAB $ ./taskA
local: 1000000, global: 1001985
local: 1000000, global: 1030758
```
### Task B

```bash
student@raspberrypi:~/group21/ex4/taskAB $ ./taskB
local: 1000000, global: 1000000
==================
WARNING: ThreadSanitizer: data race (pid=1093)
  Write of size 8 at 0x005569aa34e0 by thread T1:
    #0 fn /home/student/group21/ex4/taskAB/main.c:13:15 (taskB+0xfaf98) (BuildId: 6d1912205fc40c9b72edeec11536a4143824a6d5)

  Previous write of size 8 at 0x005569aa34e0 by thread T2:
    #0 fn /home/student/group21/ex4/taskAB/main.c:13:15 (taskB+0xfaf98) (BuildId: 6d1912205fc40c9b72edeec11536a4143824a6d5)

  Location is global 'global' of size 8 at 0x005569aa34e0 (taskB+0x15834e0)

  Thread T1 (tid=1095, running) created by main thread at:
    #0 pthread_create <null> (taskB+0x6b410) (BuildId: 6d1912205fc40c9b72edeec11536a4143824a6d5)
    #1 main /home/student/group21/ex4/taskAB/main.c:26:5 (taskB+0xfb06c) (BuildId: 6d1912205fc40c9b72edeec11536a4143824a6d5)

  Thread T2 (tid=1096, running) created by main thread at:
    #0 pthread_create <null> (taskB+0x6b410) (BuildId: 6d1912205fc40c9b72edeec11536a4143824a6d5)
    #1 main /home/student/group21/ex4/taskAB/main.c:27:5 (taskB+0xfb084) (BuildId: 6d1912205fc40c9b72edeec11536a4143824a6d5)

SUMMARY: ThreadSanitizer: data race /home/student/group21/ex4/taskAB/main.c:13:15 in fn
==================
local: 1000000, global: 1000621
ThreadSanitizer: reported 1 warnings
```
The problem was both threads accessing the global variable at the same time(race condition).
### Task C

```bash
student@raspberrypi:~/group21/ex4/taskAB $ time ./taskA
local: 1000000, global: 991057
local: 1000000, global: 1000000

real	0m0.007s
user	0m0.008s
sys	    0m0.004s
```
```bash
student@raspberrypi:~/group21/ex4/taskAB $ time ./taskB
local: 1000000, global: 1001544
ThreadSanitizer: reported 1 warnings

real	0m0.177s
user	0m0.061s
sys	    0m0.051s

```


```bash
student@raspberrypi:~/group21/ex4/taskC $ time ./taskC
local: 1000000, global: 1519073
local: 1000000, global: 2000000

real	0m0.259s
user	0m0.489s
sys	    0m0.000s
```

**Which one uses more system time, and why?**
taskB with sanitizer uses the most system time, we guess because of the sanitiizer that has to check every address and thread to detect anomalies.

**Which one uses more user time, and why?**
taskC uses the most user time, because of parallellism: when thread1 is waiting for thread2 because of the semaphore, this time is counted twice.

**Why is the real time used lower than the sum of the other two?**
Because of parallellism: user time is counted twice when a thread is waiting for the other one and there is noe system time.

### Task D
```bash
student@raspberrypi:~/group21/ex4/taskD $ sudo ./taskD
[sudo] password for student: 
b:1 L0: low priority waiting for sync
 
b:2 M0: med priority waiting for sync
 
b:3 H0: high priority waiting for sync
 
-------------------Start test-------------------
b:3 H1: high usleep
 
b:2 M1: med usleep
 
b:1 L1: low priority thread waits lock
 
b:1 L2: low priority thread has lock
 
b:1 L3: low priority thread runs with lock
 
b:2 M2: med priority thread runs
 
b:3 H3: high priority thread waits lock
 
b:2 M3: med priority thread runs
 
b:2 M4: med priority thread runs
 
b:2 M5: med priority thread runs
 
b:2 M6: med priority thread runs
 
b:1 L4: low priority thread runs with lock
 
b:1 L5: low priority thread runs with lock
 
b:1 L6: low priority thread return lock
 
b:3 H4: high priority thread has lock
 
b:3 H5: high priority thread runs with lock
 
b:3 H6: high priority thread runs with lock
 
b:3 H7: high priority thread return lock
 
-------------------End test-------------------
Finished

```

### Task E
```bash
student@raspberrypi:~/group21/ex4/taskE $ sudo ./taskE
[sudo] password for student: 
b:1 L0: low priority waiting for sync
 
b:2 M0: med priority waiting for sync
 
b:3 H0: high priority waiting for sync
 
-------------------Start test-------------------
b:3 H1: high usleep
 
b:2 M1: med usleep
 
b:1 L1: low priority thread waits lock
 
b:1 L2: low priority thread has lock
 
b:1 L3: low priority thread runs with lock
 
b:2 M2: med priority thread runs
 
b:3 H3: high priority thread waits lock
 
b:1 L4: low priority thread runs with lock
 
b:1 L5: low priority thread runs with lock
 
b:1 L6: low priority thread return lock
 
b:3 H4: high priority thread has lock
 
b:3 H5: high priority thread runs with lock
 
b:3 H6: high priority thread runs with lock
 
b:3 H7: high priority thread return lock
 
b:2 M3: med priority thread runs
 
b:2 M4: med priority thread runs
 
b:2 M5: med priority thread runs
 
b:2 M6: med priority thread runs
 
-------------------End test-------------------
Finished

```
### Task F
```bash
student@raspberrypi:~/group21/ex4/taskFG $ ./taskF
Initializing singleton...
Got singleton 0x7f98000f80:{0 0 0 0}
...
Got singleton 0x7f98000f80:{0 0 0 0}
Got singleton 0x7f98000f80:{1 0 0 0}
Got singleton 0x7f98000f80:{0 0 0 0}
Got singleton 0x7f98000f80:{0 0 0 0}
Got singleton 0x7f98000f80:{1 0 0 0}
Got singleton 0x7f98000f80:{1 0 0 0}
Got singleton 0x7f98000f80:{1 0 0 0}
Got singleton 0x7f98000f80:{1 0 0 0}
Got singleton 0x7f98000f80:{1 0 0 0}
Got singleton 0x7f98000f80:{0 0 0 0}
Got singleton 0x7f98000f80:{1 0 0 0}
Got singleton 0x7f98000f80:{1 0 0 0}
Got singleton 0x7f98000f80:{1 22 333 4444}
...

```

**What happens with the member variables?**
The instructions enters a data race where multiple threads access the same memory with no syncronization. Causing a weird output in the beginning.


### Task G


```bash
student@raspberrypi:~/group21/ex4/taskFG $ ./taskG
Initializing singleton...
==================
WARNING: ThreadSanitizer: data race (pid=1512)
  Write of size 8 at 0x00557aaa34e0 by thread T3:
    #0 getSingleton <null> (taskG+0xfafc4) (BuildId: 6a486f421a3670f82ab8d93220acbcf0eb98efa2)
    #1 singletonUser <null> (taskG+0xfb150) (BuildId: 6a486f421a3670f82ab8d93220acbcf0eb98efa2)

  Previous read of size 8 at 0x00557aaa34e0 by thread T2:
    #0 getSingleton <null> (taskG+0xfaf50) (BuildId: 6a486f421a3670f82ab8d93220acbcf0eb98efa2)
    #1 singletonUser <null> (taskG+0xfb150) (BuildId: 6a486f421a3670f82ab8d93220acbcf0eb98efa2)

  Location is global 'g_singleton' of size 8 at 0x00557aaa34e0 (taskG+0x15834e0)

  Thread T3 (tid=1516, running) created by main thread at:
    #0 pthread_create <null> (taskG+0x6b410) (BuildId: 6a486f421a3670f82ab8d93220acbcf0eb98efa2)
    #1 main <null> (taskG+0xfb26c) (BuildId: 6a486f421a3670f82ab8d93220acbcf0eb98efa2)

  Thread T2 (tid=1515, running) created by main thread at:
    #0 pthread_create <null> (taskG+0x6b410) (BuildId: 6a486f421a3670f82ab8d93220acbcf0eb98efa2)
    #1 main <null> (taskG+0xfb26c) (BuildId: 6a486f421a3670f82ab8d93220acbcf0eb98efa2)

SUMMARY: ThreadSanitizer: data race (/home/student/group21/ex4/taskFG/taskG+0xfafc4) (BuildId: 6a486f421a3670f82ab8d93220acbcf0eb98efa2) in getSingleton
==================
Got singleton 0x7f7fb01ec0:{1 22 333 4444}
...
Got singleton 0x7f7fb01ec0:{1 22 333 4444}
ThreadSanitizer: reported 1 warnings

```


### Task H
Why it doesn't work: every philosopher grabs the fork to their left, and then right, but every fork is now taken. We end up in deadlock.

**The fix:** if the forks are numbered 0-4 and every philosopher grabs the fork with the lowest number, the deadlock will be broken by the last philosopher who wants to grab the fork number 0, instead of fork 4. This frees up fork 4 for philosopher nr 3, who now has two forks and can start eating.

Run `./taskH` with either `deadlocks` or `fix`.