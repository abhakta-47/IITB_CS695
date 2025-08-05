# CS695 : Assignment 3

## Task 1: namespaces with system calls

In this task, we will explore various available system calls to play with the namespaces of a process. The following article will help us get started with the different system calls available: https://lwn.net/Articles/531381/

The following are the logical actions –

    Create a new child process in a new UTS and PID namespace
    Create a second child process and attach it to the UTS and PID namespace of the first child process

**Note:** System calls of interest are clone, setns, and unshare.

For the PID namespace, the namespace is determined during creation and cannot be changed. To create a new process with a new PID namespace, you need a flag that can be used with the clone system call. To create a new child process and attach it to an existing namespace, the parent process has to associate the flag with an existing process namespace. The clone system call does not have a flag to create and attach a new child process to an existing namespace.
Make changes to/via the parent process such that child2 is created in the same PID namespace as child1.

```
man 7 pid_namespaces
man 2 setns
man 2 clone
man 2 pidfd_open
man 2 unshare
```

Write code at marked portions in the file task1/namespace_prog.c

Don't make any changes in the file outside of marked portions.

The output of the program should be as follows:
(Parent hostname can be anything depending on your system)

```
----------------------------------------
Parent Process PID: 43156
Parent Hostname: cs695
----------------------------------------
Child1 Process PID: 1
Child1 Hostname: Child1Hostname
----------------------------------------
Parent Process PID: 43156
Parent Hostname: cs695
----------------------------------------
Child2 Process PID: 2
Child2 Hostname: Child1Hostname
----------------------------------------
Parent Process PID: 43156
Parent Hostname: cs695
----------------------------------------
```

## Task 2: CLI containers

In this task, we will create a simplified version of a container using command line tools. Here are some references to get started –
```
chroot
unshare
cgroup v2
```
The task is divided into 3 subtasks, each contributing new features to the container.
The provided (single) bash script `task2/simple_container.sh`, needs to be used and completed for functionality of all the 3 subtasks.

Don't make any changes in simple_container.sh outside of marked portions
Read comments and fill in commands at marked portions in simple-container.sh script.

### Subtask 2a: File sys isolation
- The first basic requirement of a container is that a user within a container should not be able to view/modify files outside the container's scope. 
- To achieve this filesystem isolation, a directory within a host system is generally assigned as the root directory of the container. The root is the starting location for all paths in a filesystem, so a container cannot explore the filesystem beyond its assigned root.
- Run the program container_prog by assigning the provided directory container_root as its root directory.
- Note that many programs are dependent on dynamic libraries for their execution. When loaded, these programs open their dependency library files at some predefined paths. For correct execution all dependency libraries for container_prog should be available at correct paths within the chrooted environment. All required dependencies should be copied/linked from within the script. The script should execute correctly given an empty container_root directory.


### Subtask 2b: PID & UTS isolation
- Based on the output of subtask 2a, the container has the same PID namespace as the host. This is an isolation violation, as a user from within a container can access/kill the process in the host. 
- In this subtask, attach a new PID and UTS namespace to the container. Within the new PID namespace, the PID of processes launched within the container should start with 1.
- Note that this is an incremental change, and the container running within the same chrooted environment as in subtask 2a.

### Subtask 2c: Resource provisioning
The third requirement of the container setup is resource-provisioning. A container should not be able to hog unrestricted resources of the system. cgroup allows fine granularity control of different resources for each process group. In this subtask, create a new cgroup that restricts the maximum CPU utilization to 50%. This cgroup should be assigned to all processes running within the container.

**Note:**

    CPU utilization within a cgroup is specified in terms of quota and period. Quota is the maximum total time on CPU cores the cgroup gets in the specified period of time. The quota can be greater than the period on a multicore system. You can refer to this article for more details. In this subtask, you are required to set the utilization limit to 50% of single core and not 50% utilization limit of all cores.
    There are significant changes between cgroup v1 and cgroup v2. You will be working with cgroup v2 in this assignment.

Read comments and fill in commands at marked portions in simple-container.sh script.

Sample output on running simple-container.sh:

### Output Subtask 2a
```
Process PID: 1693
Child Process PID: 1694
_ _ _ _ _ _ _ _ _ _ _ _ _ _ 
Files/Directories in root directory:
lib64
container_prog
.
..
lib
_ _ _ _ _ _ _ _ _ _ _ _ _ _ 
__________________________________________
```

### Output Subtask 2b
```
Process PID: 1
Child Process PID: 2
_ _ _ _ _ _ _ _ _ _ _ _ _ _ 
Files/Directories in root directory:
lib64
container_prog
.
..
lib
_ _ _ _ _ _ _ _ _ _ _ _ _ _ 

Hostname within container: new_hostname
Computation Benchmark:
Time Taken: 1440778 ms
Value (Ignore): 107375219085276240
Hostname in the host: cs695
__________________________________________
```
### Output Subtask 2c
```
Process PID: 1
Child Process PID: 2
_ _ _ _ _ _ _ _ _ _ _ _ _ _ 
Files/Directories in root directory:
lib64
container_prog
.
..
lib
_ _ _ _ _ _ _ _ _ _ _ _ _ _ 
Computation Benchmark:
Time Taken: 2866732 ms
Value (Ignore): 107375219085276240
```

## Task 3: containers in the wild

In this task, we will develop a more complete container management tool like Docker. The tool will provide the following features:

- Ability to create full fledged debian/ubuntu container images
- Instantiating new containers from an image
- Enabling the following network capabilities for the containers
    - Network path between container and host
    - Network path between container and public
    - TCP Port forwarding between host and container
    - Network path between different containers on the host
- Dockerfile-like interface to setup image with layers and overlayfs

There are two script files for this task `setup.sh` and `conductor.sh`
- `setup.sh` is used to set configuration parameters for our tool
- `conductor.sh` script is the actual tool that will perform all operations

**Important:** Within the setup.sh script set the `DEFAULT_IFC` variable to the name of the network interface connecting your VM/Machine to the external network. Without this step, you cannot access the external network from your containers. You can use ip a command to list all network interfaces.

Functions provided by the tool are documented as follows:

`sudo ./conductor.sh -h` can also be used to list out the functions of the tool

### build

**Usage:** ./conductor.sh build <image-name> <conductorfile>

**Operation:** This function will download and create an image according to a given Conductorfile (similar to Dockerfile) – the root directory structure of a system and all its sub-directories in a local directory, using the debootstrap command. The Conductorfile only support FROM instruction (by default) and can contain one of the following:
```
FROM debian:bookworm
FROM ubuntu:jammy
FROM ubuntu:focal
```

If no name for the <conductorfile> is given then it will consider the “Conductorfile” named file as its input.


**Note:** debootstrapping takes up lots of time. That's why the build file system for debootstrap happens in .cache/base directory and images just copy the base to ./images/<image-name> directory.

### images

**Usage:** ./conductor.sh images
**Operation:** This function will list all container images available in the configured images directory.

### rmi

**Usage:** ./conductor.sh rmi <image-name>
**Operation:** This function will delete the given image from the configured images directory.

### rmcache

**Usage:** ./conductor.sh rmcache
**Operation:** This function will delete all cache including base images.
(Proceed with caution)

### run [this functionality needs implementation]

**Usage:** ./conductor.sh <image-name> <container-name> --[command <args>]
**Operation:** This function will start a new container with the name as container-name from the specified image. command <args> is the program and its arguments (if any) that will be executed within the container as the first process. If no command is given, it will execute /bin/bash by default. The container will have isolated UTS, PID, NET, MOUNT and IPC namespaces. It will also mount the following file systems within the container:

    procfs for tools dependent on it like ps, top etc. to work correctly
    sysfs to be able to setup ip forwarding using iproute2 tool
    /dev (in host) bind mount to container-rootfs/dev to enable the container to see network devices

The first process will have pid = 1 in the container. It will also set correct file permission on the container-rootfs/ so that tools like apt work correctly. A directory named container-name within the configured CONTAINERDIR directory will be created. Furthermore container-name will have a subdirectory named rootfs which will be mounted as root directory for the container.

**Note:** If you run a container and exit from it, you cannot run the same container again. First you will need to delete the container using stop command, then run it.

### ps

**Usage:** ./conductor.sh ps
**Operation:** This function will show all running containers by querying entries within the configured CONTAINERDIR directory.

### stop

**Usage:** ./conductor.sh stop <container-name>
**Operation:** This function will stop a running container with a given name. Stopping a container involves:

    Killing the unshare process that started the container
    Killing all processes running within the container
    Unmounting any remaining mount point within the container rootfs.
    Deleting container-name with configured CONTAINERDIR directory.

**Note:** Stopping a container will delete all state of the container

exec [this functionality needs implementation]

**Usage:** ./conductor.sh exec <container-name> –-[command <args>]
**Operation:** This function executes the given program along with its arguments within the specified running container. If no command is provided it will execute /bin/bash by default. The executed program will be in the same UTS, PID, NET, MOUNT and IPC namespace as the specified container. Furthermore it will see the root directory as conatiner-name/rootfs. It will also see the same procfs, sysfs and /dev filesystem as configured within the container and tools like ps, top etc. should work correctly.

addnetwork [this functionality needs implementation]

**Usage:** ./conductor.sh addnetwork <container-name> [options]
**Operation:** This function will add a network interface to a container and setup its configurations so that the container can communicate using the network.

By default if no options is given it will setup the container network as shown in the diagram below:

+--------------------------------+                  +-----------------+

| Host                           |                  | container 'eg'  |

|                                |   From 'host'    |                 |

+-----------+       +------------+ TX->        RX-> +------------+    |

+ default   |       | eg-outside +------------------+  eg-inside |    |

+ interface |       +------------+ <-RX        <-TX +------------+    |

+---------- +                    |    From 'eg'     |                 |

|                                |                  |                 |

+--------------------------------+                  +-----------------+

It should be noted that only communication between eg-outside and eg-inside is possible. That implies that an application running inside a container eg will not be able to access the Internet (end points beyond-outside.)

If the option -i or --internet is specified the script should allow for the applications running inside the container to access the Internet. The schematic diagram for the same is shown below:

+--------------------------------+                  +-----------------+

| Host                           |                  | container 'eg'  |

|                                |   From 'host'    |                 |

+-----------+       +------------+ TX->        RX-> +------------+    |

+ default   |--NAT--| eg-outside +------------------+  eg-inside |    |

+ interface |       +------------+ <-RX        <-TX +------------+    |

+---------- +                    |    From 'eg'     |                 |

|                                |                  |                 |

+--------------------------------+                  +-----------------+

Although the -i option allows Internet usage, exposing services deployed inside containers is still not possible (Basically, if you deploy a server or anything inside the container it will not be accessible outside the host).

You can use the --expose or -e option to make a port available to services outside of the container. This creates a rule in the host, mapping a container port to a port on the host to the outside world. Here are some examples:

./sudo conductor.sh addnetwork -e 8080-80 : Map port 80 on the host to TCP port 8080 in the container.

peer

**Usage:** ./conductor.sh peer <container1-name> <container2-name>
**Operation:** By default our conductor isolates the container so that no inter-container communication is possible. This function allows two containers to communicate with each other.

The skeleton code implements most of the functionalities of the tool. For Task 3 you need to do the following:

    Understand the working of the tool. To get started the skeleton code has portions marked as Lesson along with some comments.
    Implementation of some parts of the tool is missing. The required implementations are marked as Subtask in the skeleton code.
    You are free to modify any part of the bash script to make it better, considering it does not break any functionality. 

Initial Setup

You will be required to install debootstrap and iptables tools for the script to work. You can install them with

sudo apt install debootstrap iptables

Subtask 3.a: Implement run

Implement run to use unshare and chroot to run a container from a given image. You also need to mount appropriate filesystems to the rootfs within the container to enable tools that utilize those filesystems e.g. ps, top, ifconfig etc. to be confined within the container isolation.

Considering you already have an image ready for usage, the sample outputs of the tool if you have set up everything properly is shown below:

cs695@cs695:~/task3$ sudo ./conductor.sh images

CS695 Conductor that manages containers

Name                 Size       Date

testimage            307M       2025-03-20

cs695@cs695:~/task3$ sudo ./conductor.sh ps

CS695 Conductor that manages containers

No containers found

cs695@cs695:~/task3$ sudo ./conductor.sh run testimage eg

CS695 Conductor that manages containers

root@cs695:/# ps

    PID TTY          TIME CMD

      1 ?        00:00:00 bash

      3 ?        00:00:00 ps

root@cs695:/# exit

exit

cs695@cs695:~/task3$ sudo ./conductor.sh ps

CS695 Conductor that manages containers

Name                 Date

eg                   2025-03-20

cs695@cs695:~/task3$ sudo ./conductor.sh stop eg

CS695 Conductor that manages containers

eg succesfully removed

cs695@cs695:~/task3$ sudo ./conductor.sh ps

CS695 Conductor that manages containers

No containers found

Subtask 3.b: Implement exec

You need to complete the implementation of exec which can execute a command to join the existing namespace {all namespaces: uts, pid, net, mount, ipc} of the running container and execute the given command and args. The executed process should be within the correct namespace and root directory of the container and tools like ps, top should show only processes running within the container.

The following output shows the correct execution of the tool if the implementation is done properly and there exists a container eg which is running.

cs695@cs695:~/task3$ sudo ./conductor.sh ps

CS695 Conductor that manages containers

Name                 Date     

eg                   2025-03-20

cs695@cs695:~/task3$ sudo ./conductor.sh exec eg /bin/bash

CS695 Conductor that manages containers

Executing exec in eg container!

root@cs695:/# ps aef

    PID TTY      STAT   TIME COMMAND

      4 ?        S      0:00 /bin/bash 

      5 ?        R+     0:00  \_ ps aef

      1 ?        S      0:00 /bin/bash

      3 ?        S+     0:00  \_ sleep inf

Subtask 3.c: Implement networking

Although most of the networking configuration is already being done in the tool. But one of the most important tasks i.e adding the interface that does the communication is missing from the implementation. For the subtask you need to add a veth link (It is a peer link connecting two points) connecting the container's network namespace to the root (host) namespace. The veth link will have two interfaces. Moreover the interfaces should be enabled for its proper usage.

If everything is done properly the output of the container and the host should be as follows:

    Container side:

cs695@cs695:~/task3$ sudo ./conductor.sh run testimage eg

CS695 Conductor that manages containers

root@cs695:/# ip a

1: lo: <LOOPBACK> mtu 65536 qdisc noop state DOWN group default qlen 1000

    link/loopback 00:00:00:00:00:00 brd 00:00:00:00:00:00

root@cs695:/# ip a

1: lo: <LOOPBACK,UP,LOWER_UP> mtu 65536 qdisc noqueue state UNKNOWN group default qlen 1000

    link/loopback 00:00:00:00:00:00 brd 00:00:00:00:00:00

    inet 127.0.0.1/8 scope host lo

       valid_lft forever preferred_lft forever

    inet6 ::1/128 scope host 

       valid_lft forever preferred_lft forever

2: eg-inside@if5: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc noqueue state UP group default qlen 1000

    link/ether ca:09:b2:0c:be:ee brd ff:ff:ff:ff:ff:ff link-netnsid 0

    inet 192.168.1.2/24 scope global eg-inside

       valid_lft forever preferred_lft forever

    inet6 fe80::c809:b2ff:fe0c:beee/64 scope link 

       valid_lft forever preferred_lft forever

root@cs695:/# ping 192.168.1.1

PING 192.168.1.1 (192.168.1.1) 56(84) bytes of data.

64 bytes from 192.168.1.1: icmp_seq=1 ttl=64 time=0.151 ms

64 bytes from 192.168.1.1: icmp_seq=2 ttl=64 time=0.085 ms

^C

--- 192.168.1.1 ping statistics ---

2 packets transmitted, 2 received, 0% packet loss, time 1015ms

rtt min/avg/max/mdev = 0.085/0.118/0.151/0.033 ms

    Host side:

cs695@cs695:~/task3$ sudo ./conductor.sh addnetwork eg

CS695 Conductor that manages containers

Setting up network 'eg' with peer ip 192.168.1.2. Waiting for interface configuration to settle...

cs695@cs695:~/task3$ ip a

1: lo: <LOOPBACK,UP,LOWER_UP> mtu 65536 qdisc noqueue state UNKNOWN group default qlen 1000

    link/loopback 00:00:00:00:00:00 brd 00:00:00:00:00:00

    inet 127.0.0.1/8 scope host lo

       valid_lft forever preferred_lft forever

    inet6 ::1/128 scope host noprefixroute 

       valid_lft forever preferred_lft forever

2: enp1s0: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc fq_codel state UP group default qlen 1000

    link/ether 52:54:00:a3:b7:09 brd ff:ff:ff:ff:ff:ff

    inet <hidden-for-safety> brd <hidden-for-safety> scope global dynamic enp1s0

       valid_lft 8167sec preferred_lft 8167sec

    inet6 <hidden-for-safety> scope link 

       valid_lft forever preferred_lft forever

5: eg-outside@if2: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc noqueue state UP group default qlen 1000

    link/ether d2:69:7d:88:da:38 brd ff:ff:ff:ff:ff:ff link-netnsid 0

    inet 192.168.1.1/24 scope global eg-outside

       valid_lft forever preferred_lft forever

    inet6 fe80::d069:7dff:fe88:da38/64 scope link 

       valid_lft forever preferred_lft forever

Subtask 3.d: Implement overlay filesystem

Currently, whenever a new image or container is created, it copies the root directory again and again, wasting a lot of storage. In this task, you have to implement a new filesystem over the base layer using overlayfs. Learn more about overlayfs here:
https://wiki.archlinux.org/title/Overlay_filesystem

In this subtask, you need to do the following steps:

    Remove copying of the base root directory to the image directory, Instead, consider the base root directory as a layer of overlay fs and write its address in a file in the image directory. Name the file ‘layers’
    Remove copying of the image root directory to the container directory, instead, read the layer from the ‘layers’ file of the image directory and use it for mounting overlayfs of the container.
    Modify all the mount and container interaction operations in the tool to use the new overlay merged file directory instead of previously used rootfs.
    When the container stops, the overlay file system should also be unmounted.

**Note:** When a container is running, only one overlayfs should be mounted. 

Subtask 3.e: Implement COPY instruction for Conductorfile

Currently, the Conductor file only supports FROM instruction, you should extend the tool to support COPY instruction. Since the instruction modifies the rootfs, you should consider it as a layer for the overlayfs and store it in the layer stack.

To implement the COPY instruction, you need to do the following:

    Create a new layer for the COPY instruction.
    Temporarily mount the overlayfs
    Perform the copy operation
    Unmount the overlay file system

Subtask 3.f: Implement RUN instruction for Conductorfile

Now extend your tool to support RUN instruction just like Dockerfile.

To implement the RUN instruction you need to do the following:

    Create a new layer for the RUN instruction
    Temporarily mount the overlayfs
    Execute the RUN instruction properly inside the overlay filesystem.
    Clean up and record metadata

Task 4: creating the matrix

The main motivation of the previous task was to understand how tools like Docker, podman, lxc, etc. work internally with the help of Linux tools and primitives. But why should we bother about containers?

This task is meant to answer the question (partially, if not completely). The task requires us to deploy two services namely the external-service and the counter-service, in two separate containers. Only the external-service is accessible from the outside world (other hosts on the network), whereas the counter-service should only be accessible from the host. We will be using the conductor tool that was implemented in Task 3 and use a service-orchestrator script to create the matrix!

Follow the following steps to implement task 4 in the service-orchestrator script:

    Write two Conductorfiles for two images, say es-image and cs-image.
    The Conductorfile should do the following:

    Take any base image e.g. FROM debian:bookworm
    Copy directory external-service to image es-image and counter-service to image cs-image at appropriate location.
    Using RUN instruction in cs-image go inside the appropriate location, perform the following:

    apt update
    apt install -y build-essential
    make

and in es-image go inside the appropriate location, perform the following:

    apt update
    apt install -y python3 python3-flask python3-requests

    Build the images for the containers using the appropriate Conductorfiles.
    Run two containers, say  cs-cont and es-cont which should run in the background. You can add some sleep in the script so that c1 and c2 are running before the next commands are executed. (Not more than 2 seconds)

Tip: To keep the container running in background use a first program that will not interact with the terminal and will not exit. e.g. sleep infinity, tail -f /dev/null

    Configure network such that:
    4.a. es-cont is connected to the internet and es-cont has its port 8080 forwarded to port 3000 of the host
    4.b: cs-cont is connected to the internet and does not have any port exposed
    4.c: peer network is setup between es-cont and cs-cont
    Get IP address of cs-cont. You should use script to get the IP address – can use ip interface configuration within the host to get IP address of cs-cont or can exec any command within cs-cont to get it's IP address
    Within cs-cont launch the counter service using exec cs-cont -- [path to counter-service directory within cs-cont]/counter-service 8080 1
    Within es-cont launch the external service using exec es-cont -- python3 [path to external-service directory within es-cont]/app.py "http://<ipaddr_cs-cont>:8080/"
    Within the host system open/curl the url: http://<host-ip>:3000 to verify output of the service. **Note:** http://localhost:3000 from within the host will not work.
    On any system which can ping the host system open/curl the url: http://<host-ip>:3000 to verify output of the service

Frequently Asked Questions

Q. I am unable to do an apt update or apt install within the VM. What to do?
A: Make sure you are connected to the Internet. If you are inside IITB campus you can use the following bash script to login to IITB internet using the terminal. This is not required for the containers as they are using NAT and thus using VM's IP to connect to the internet.

Q. Something went wrong. I want to make the setup clean as start.
A: Perform stop operation on all container and remove the image using rmi to make a clean start. Sometimes, doing rm -rf .cache/layer might also be necessary.

Q. Why I am unable to curl http://<host-ip>:3000?
A: Make sure you firewall is not blocking the port. Update firewall rules or disable the firewall entirely (Not recommended). Also, if your VM network is configured to use NAT, then you will need to forward port 3000 of your VM to the host.
Submission details

    Please follow the following format for submitting your assignment
    Verify the tar file before submitting.

The submission tar should have following format.

<rollnumber>_assignment3/

          │

          ├──── .git/

          │        └── ... /* all git-related files */

          │

          ├──── task1/

          │        └── namespace_prog.c

          │

          ├──── task2/

          │        └── simple_container.sh

          │

          ├──── task3/

          │        ├── conductor.sh

          │        └── setup.sh

          │

          └──── task4/

                   ├── csfile

                   ├── esfile

                   └── service-orchestrator.sh

Submission Deadline: 31st March 2025, 11.59 pm via Moodle