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

# Task 2: IOCTL
## Task 2.1 Memory operations
### IOCTL driver implementatin
1. Provide the physical address for a given virtual address. The virtual to physical-address translation should be for the current running process.
2. Given a list of physical addresses and corresponding byte values, write the specified byte value to each of the provided physical memory addresses. The physical address and the value to be written as parameters of the ioctl call.
### User Space program
1. Allocates a memory address block of size count  bytes on the heap
2. Assign the value “104” to the first byte of the allocated memory address
3. Assign a value, Incremented by 1, for each consecutive memory address until the entire block is filled.
4. Print the addresses (virtual addresses) of the allocated memory and their corresponding values.  
5. Make an ioctl call to get the physical addresses of the allocated memory. Print the physical addresses.
6. Implement another ioctl call to update the value at the first memory address to 53 and increment the values at subsequent addresses by 1, using their respective physical memory addresses
7. Verify the modified values by printing the content of the allocated memory block.
### spock.sh script
1. compiles your ioctl driver and user space application
2. initialize the ioctl device
3. runs your user space application
4. cleanly removes the ioctl device
##  Task 2.2 
### Sub Task 2.2.A
Implement the following ioctl device driver which provides a call with the following specifications:
1. Takes a pid as an argument and modifies the task structure of the current process to change its parent process with the given pid. More specifically, the process with the given pid should receive a SIGCHLD signal on exit of the current process (which makes the ioctl call).
This IOCTL call/functionality is used by the foot soldier program to attach itself to the control station.
2. A process use pid as an argument and terminates all child processes of the given pid. After all child processes get terminated, the process terminates/exits itself. This ioctl call is done on an emergency
This second IOCTL call/functionality is used by the control station to initiate a reset condition to stop/terminate the foot soldier programs.
### Sub Task 2.2.B
The soldier and control station programs to invoke the ioctl calls, and a script that launches these programs are provided at this link. Modify the script at the mentioned places to compile, initiate, and remove the ioctl device.

Sample Output:
```bash
cs695@cs695:~$ ./run_dr_bloom.sh
Control station PID: 12148
Soldier PID: 12149
Soldier PID: 12150
Soldier PID: 12151
Soldier PID: 12152
[CHILD]: soldier 12152 changing its parent
[CHILD]: soldier 12151 changing its parent
[CHILD]: soldier 12150 changing its parent
[CHILD]: soldier 12149 changing its parent
[PARENT]: Control station process 12148 started
[PARENT]: Soldier process 12150 terminated
[PARENT]: Soldier process 12149 terminated
[PARENT]: Emergency Emergency!
[PARENT]: Soldier process 12152 terminated
[PARENT]: Soldier process 12151 terminated
[PARENT]: Control station 12148 exiting                 
```
# Task 3: procfs and sysfs
## Task 3.1
Develop a kernel module named get_pgfaults.c with the objective of introducing a fresh entry into the /proc filesystem, specifically labeled /proc/get_pgfaults. Users should be able to use commands like cat to retrieve and display information regarding the total count of page faults the operating system has encountered since it booted.
## Task 3.2 
Write a kernel module get_memstats.c that creates a /sys/kernel/mem_stats directory to expose memory statistics similar to Task 1.4. The directory should have the following four files:
1. pid: The pid of the process for which the memory statistics should be shown. The file should have both read and write and access and contain a default value of -1.
2. virtmem: The current virtual memory size of the process represented by the pid. This file should only have read access and, by default, should show the values in Bytes.
3. physmem: The current physical memory of the process represented by the pid. Similar to virtmem the file should also be read-only, and by default should show the values in Bytes.
4. unit: Unit of memory that should be shown in the virtmem and physmem files. This file is both readable and writable. The file can take three inputs “B” representing Bytes, “K” representing KB, and “M” representing MB. On kernel module loading by default the file should contain “B” and should be changeable at runtime. All other values should be discarded appropriately.

# ToDos
- [ ] Task 1
    - [x] Task 1.1
    - [x] Task 1.2
    - [ ] Task 1.3 : graph etc
    - [x] Task 1.4
    - [ ] Task 1.5 : test script
- [.] Task 2
    - [x] Task 2.1
    - [.] Task 2.2
- [.] Tasl 3
    - [ ] Task 3.1
    - [x] Task 3.2