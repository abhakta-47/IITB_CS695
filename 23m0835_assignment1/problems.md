# Task 1

## Task 1.1
Write a kernel module to list all processes in a running or runnable state. Print their pid and process name. Name the source file as lkm1.c
## Task 1.2
Write a kernel module that takes process ID as input [input should be named pid] and prints their pid and process state for each of its child processes. Name the source file as lkm2.c
## Task 1.3 
Write a kernel module that takes process ID [input should be named pid] and a virtual address (in decimal) [input should be named vaddr] as its input. Determine if the virtual address is mapped, and if so, determine its physical address (pseudo physical address if running in a VM). In case of an unmapped address, proper messages should be printed otherwise, print the PID, virtual address, and corresponding physical address. Name the source file as lkm3.c
## Task 1.4
For a specified process (via its PID) [input should be named pid], determine the size of the allocated virtual address space (sum of all vmas) and the mapped physical address space. Write a test program test1.c, that takes the number of pages to be allocated and a stride as CLI arguments. The program should first allocate the required pages and start from the first allocated address, write using that address, and continue writing at the next address, which is stride bytes away from the current address till the end of the allocated region. For example, an allocation of 128MB and a stride of 16MB will result in 8 writes.
Observe the mapped memory stats using your LKM for different combinations of the input tuple. Note that allocation of memory and accessing memory are two different actions. Write a short note on your observation in lkm4.txt and draw a graph with allocated pages on the x-axis, the mapped physical memory size of the process for different stride values. Make sure that you allocate memory in the granularity of page size. Name the source file as lkm4.c and lkm4.png for the graph.
Sample Message Log:
```log
[10410.354845] [LKM4] Virtual Memory Size: 10656 KiB
[10410.354895] [LKM4] Physical Memory Size: 1512 KiB
[10426.035585] [LKM4] Module LKM4 Unloaded
[10667.045042] [LKM4] Virtual Memory Size: 10656 KiB
[10667.045068] [LKM4] Physical Memory Size: 3548 KiB
[10830.091668] [LKM4] Module LKM4 Unloaded
```
## Task 1.5 
In the Linux Kernel, the page size is usually 4K, but applications dealing with large memory working sets usually use huge page sizes (2M, 1G [may vary with architecture]) to optimize its performance. One way to use hugepages is to allocate anonymous memory and enable Transparent HugePage Support (THP) to promote and demote pages to different sizes automatically. Write a test program test2.c, that allocates anonymous memory of reasonably large size. For the specified process (via its PID) [input should be named pid], determine the number of huge pages and the allocated virtual address space they use when THP is enabled and when it is disabled. The source file should be named lkm5.c
Sample Message Log:
```log
[114958.052158] [LKM5] THP Size: 202752 KiB, THP count: 99
[114970.326244] [LKM5] Module LKM5 Unloaded
[115056.042726] [LKM5] THP Size: 0 KiB, THP count: 0
[115139.727883] [LKM5] Module LKM5 Unloaded
```