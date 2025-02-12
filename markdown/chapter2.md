## What happens when a program runs?

- A running program executes instructions.
    - The processor fetches an instruction from memory
    - decodes the instruction
    - executes the instruction.
    
- This process repeats. This approcah describes the **Von Neumann** model.

## What is an Operating System

- We want some sort of program that needs to make:
    - running programs easy
    - be able to share memory
    - allow programs to interact with devices.

- This software is known as the Operating System.

## What is virtualization

- Virtualization is the act of the OS taking a physical resource (cpu, memory, disk), and turning it into an easier to use virtual form of itself.

## How do we instruct the OS?

- We use system calls, which is an API the OS provides.
- we also call this a **standard library** to applications.

## OS Role - Resource Management

- The OS is also responsible for managing resources.

- Your cpu, memory and disk is a resource of the system.
    - The OS must guarantee that it can efficiently and fairly manage the resources.


## CPU Virtualization

```c
#include<stdio.h>
#include<stdlib.h>
#include<sys/time.h>
#include "common.h"

int main(int argc, char** argv){
    while(1){
        Spin(1);
        printf("%s\n",argv[1]);
    }
}
```

```bash
prompt> ./cpu A & ./cpu B & ./cpu C & ./cpu D &
[1] 7353
[2] 7354
[3] 7355
[4] 7356
A
B
D
C
A
B
D
C
A
```
- If we run multiple instances of this program, you can see that it runs seemingly concurrently.

- CPU Virtualization gives the illusion that there are a lot of virtual CPUs, despite only having one physical CPU.


## Memory Virtualization

- physical memory is simply an array of byes.
    - To read and write/update from memory, we specify an address to access.

```bash
prompt> ./mem & ./mem &
[1] 24113
[2] 24114
(24113) address pointed to by p: 0x200000
(24114) address pointed to by p: 0x200000
(24113) p: 1
(24114) p: 1
(24114) p: 2
(24113) p: 2
(24113) p: 3
(24114) p: 3
(24113) p: 4
(24114) p: 4
```

- We can see that the two memory addresses are the same, but the process have different values.
- These addresses aren't actually located at 0x200000.

- Your OS is virtualizing memory, where each process has its own private virtual address space.

## Concurrency

- Concurrency is the ability to run multiple things simultaneously.

```c
#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "common_threads.h"

volatile int counter = 0;
int loops;

void *worker(void *arg) {
    int i;
    for (i = 0; i < loops; i++) {
        counter++;
    }
    return NULL;
    }
     
int main(int argc, char *argv[]) {
     if (argc != 2) {
         fprintf(stderr, "usage: threads <value>\n");
         exit(1);
     }
     loops = atoi(argv[1]);
     pthread_t p1, p2;
     printf("Initial value : %d\n", counter);
    
     Pthread_create(&p1, NULL, worker, NULL);
     Pthread_create(&p2, NULL, worker, NULL);
     Pthread_join(p1, NULL);
     Pthread_join(p2, NULL);
     printf("Final value : %d\n", counter);
     return 0;
 }
```

- For example, we're running the function worker simultaneously.
- The issues that exist with concurrency doesn't lie in the OS it's self, but with multithreading itself.
- This program seems alright, but you can see the result of the program:

```bash
prompt> ./threads 100000
Initial value : 0
Final value : 143012 // huh??
prompt> ./threads 100000
Initial value : 0
Final value : 137298 // what the??
```

## Persistence

- Problem about ram is that you lose data in dram when the device is turned off.
- We need hardware and software to be able to store software persistently.
    - These are hard-drives, SSDs.
- We also need software to manage the disk. This is known as the file system.

```c
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <fcntl.h>
#include <sys/types.h>

int main(int argc, char *argv[]) {
    int fd = open("/tmp/file", O_WRONLY|O_CREAT|O_TRUNC, S_IRWXU);
    assert(fd > -1);
    int rc = write(fd, "hello world\n", 13);
    assert(rc == 13);
    close(fd);
    return 0;
}
```
- This program wants to open a file `/tmp/file`.
- To do this, the program makes three system calls to the OS:
    - `open()`
    - `write`
    - `close()`
- The file system is responsible for handling the requests and returning some sort of error code.
