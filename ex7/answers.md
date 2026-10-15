# Lab 7: RPi and butterfly

### Task A
Fast af

### Task B
Had to yield in order for the program to run completely.


### Task C
System can only handle two disturbances before timing out with max timeout 8ms, can handle 10 with timeout 65ms.

### Task D
With no disturbances, the busy-polling techique has lower WCET and mean response times.

With 2 disturbances, the busy-polling has much higher change in reponse times than periodic, making periodic much more predictable. 
For reference, busy-polling has a baseline of around 33 micro-seconds WCET, but that rises to around 8000 micro-seconds with 2 disturances.
Periodic excecution has a baseline of 2019us WCET, and 7446us with 2 disturbances.