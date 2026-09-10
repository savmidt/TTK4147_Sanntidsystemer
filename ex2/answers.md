## Task A:
#### What happens?
The memory becomes full, and swap starts filling up. One of the CPU cores goes full throttle allocating the space.
Baseline: around 100 Mb memory, 40 mb swap, after running program: memory is full, 3.59Gb, and 412 Mb of swap.

#### What is the difference between "Memory" and "Swap"?
Memory is in he actual RAM sticks, while swap is when the system memory is full, the OS will send the memory of processes that are paused to storage medium of the PC.

After changing x_dim= 1000:
Memory goes to 2Gb ish. No swap

Changing x_dim = 10000:
Memory goes to 332Mb, no swap.

Using memset to set all the values to 0:
baseline: 100 Mb memory, 36 mb swap
maximum memory, 290Mb swap


# Task B
#### array_insertback implemented:
student@raspberrypi:~/group21/ex2/taskB $ ./taskB
Array:{}
Array:{1, 1, 1, 1, 3565507822994223681}

#### After compiler flags:
student@raspberrypi:~/group21/ex2/taskB $ ./taskB
AddressSanitizer: CHECK failed: sanitizer_allocator_primary64.h:131 "((kSpaceBeg)) == ((address_range.Init(TotalSpaceSize, PrimaryAllocatorName, kSpaceBeg)))" (0x500000000000, 0xfffffffffffffff4) (tid=2785)
    <empty stack>

#### Task Changing
fixed insertBack:
student@raspberrypi:~/group21/ex2/taskC $ ./taskC
Array:{}
Array:{1, 1, 1, 1, 1}


# Task C
Running, works!

# Task D
When insertBack is called and the array does not have enough space, it will allocate double or 1.5 times the current capacity.
Exception: if existing capacity is 0, the new capacity is 1.

#### In what cases will you be able to reuse memory that you have freed up?
Will not be able to use reuse memory when using the pop-functions. However the reserve function will free up the previously used space.