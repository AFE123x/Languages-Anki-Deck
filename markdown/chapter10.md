# 10. Kernel Synchronization Methods

## Atomic Operations, atomic_t type, atomic_t operations

- Atomic operations provide instructions that execute automically, without interruptions.
    - Similar to how atoms were originally thought to be indivisible, atomic operators are indivisible, and can't be interrupted.
- There are a few atomic instructions, like test-and-set, increment a variable by one, etc.
- The kernel provides two interfaces for atomic operations:
    - One that operates on integers
    - Another that operates on individual bits.

### atomic_t type, atomic int operations
- defined in `<linux/types.h>`
- We represent atomic integers with the `atomic_t` type.
    - We use this so the compiler doesn't decide to foolishly optimize it.
- We have a fair share of atomic operations:

```c
atomic_t v; /* defines v */
atomic_t u = ATOMIC_INIT(0); /* defines u and initializes it to 0 */

/* operations */
atomic_set(&v, 4); /* atomically sets v to 4. */
atomic_add(2, &v); /* atomically adds 2 to v. */
atomic_inc(&v); /* atomically increments v by 1. */

printk("%d\n",atomic_read(&v)); /* atomically reads to int in v. */

if(!atomic_dec_and_test(&v)){printk("v is zero!!!\n");} /* misc. operations included in <asm/atomic.h>
```

### 64 bit version

- The 64 bit version of the `atomic_t` type is `atomic64_t` type.


## atomic bitwise operations

- In addition to the integer operations, you can also perform bit manipulation.

```c
unsigned long word = 0; //no special atomic type.
set_bit(0,&word); /* atomically sets the 0th bit on word to 1. */
clear_bit(0, &word) /* atomically sets the 0th bit on word to 0 */
```

## Spin Locks

- A spin lock is a lock that can be held by one thread of execution.
    - If another thread tries to acquire the lock, it'll be stuck it a loop (hence spin) until it can acquire the lock.

### spin lock methods

- Spin locks can be architecture dependent:
    - The architecture-dependent version is in `<asm/spinlock.h>`
    - The usable interface is defined in `<linux/spinlock.h>`

- the basical interface of a spinlock is as follows:

```c
DEFINE_SPINLOCK(mr_lock)

spin_lock(&mr_lock);
/* critical region */
spin_unlock(&mr_lock);
```

## Spin locks on interrupt handlers

- Remember the one rule about interrupt handlers: they can't sleep.
- If you want to use a spinlock in the interrupt handler, you need to disable local interrupts before obtaining the lock.
    - This is to ensure another interrupt handler doesn't attempt to reacquire the lock.

- The linux kernel provides a nice interface for this:

```c
DEFINE_SPINLOCK(mr_lock);
unsigned long flag;

spin_lock_irqsave(&mr_lock, flags);
/* critical region */
spin_unlock_irqrestore(&mr_lock, flags);
```
- This obtains the lock, disable interrupts locally, and save the current state of the interrupts.

## Spin Locks on Bottom halves

- You can dalso have spinlocks in the botom half:

```c
DEFINE_SPINLOCK(mr_lock);

spin_lock_bh(&mr_lock);
/* Critical Section */
spin_unlock_bh(&mr_lock);
```

## Reader-Writer Spin Locks

- Sometimes, we can divide threads into reader and writer threads.
    - For example, one thread reads the contents of a queue, the writer adds to the queue. This is the reader/writer problem, or the consumer/producer problem
- The kernel offers a read_write lock.

```c
DEFINE_RWLOCK(mr_rwlock);

void reader_thread(){
    read_lock(&mr_rwlock);
    /* critical section (read only) */
    read_unlock(&mr_rwlock);
}

void writer_thread(){
    write_lock(&mr_rwlock);
    /* critical section (read and write) */
    write_unlock(&mr_rwlock);
}

```

- if you have 5 readlocks, they can have all five locks simultaneously, since they're just reading.
- you can only have a single write lock, since you're writing data.


## Semaphores, conclusions

- Spinlocks wait in a loop until it can acquire the lock.
- Semaphores are known as sleeping locks.
    - This means, if the semaphore is unavailable, the task is put on a waiting queue and put to sleep, letting other tasks to complete.
- The the semaphore is available, one of the tasks on the wait queue is woken up, and acquires the semaphore.


### conclusions

- Semaphores are great for tasks that take a long time to finish.
    - This means semaphores aren't great for short tasks, since there's overhead associated with sleeping and waking up tasks.
- Semaphores only work on process context since interrupt context aren't schedulable.


## Semaphores - Counting

- Semaphores allow for a arbitrary number of simultaneous lock holders.

## Creating and Initializing Semaphores

- Semaphores are defined in `<asm/semaphore.h>`

- Semaphores are represented with the semaphore struct, and initialized with `sema_init`

```c
struct semaphore name;
int count = 5; 
sema_init(&name, count); //semaphore that allows for 5 lock owners.
```

## Semaphore - Using

```c
struct semaphore sem_san;
sema_init(&sem_san, 1);//defines a semaphore that can have one lock holder

if(down_interruptable(&sem_san)){
    /* error handling */
}
/* critical region */
up(&sem_san);
```

- you can use `down()` or `down_interruptable()`, but `down_interruptable()` is preferred.
    - `down()` will sleep through signals
    - `down_interruptable()` will sleep, but wake up to signals.


## Reader-Writer Semaphores - initializing, locking/unlocking

- read-write semaphores are defined in the `<linux/rwsem.h>` header file.

### Initializing
- You can declare the read-write semaphore with the declare_rwsem macro:

```c
static DECLARE_RWSEM(name);
```

- If you choose to initialize in dynamically, you can you `init_rwsem(&name)` to initialize your semaphore.
- all read_write semaphores are mutexes, so their usage count is one
    - Similar to read-write locks, the locks are enforced only for the writers, not readers.
    - You can have as many readers, but only one writer (with no readers).

### locking/unlocking

```c
static DECLARE_RWSEM(mr_rwsem);

down_read(&mr_rwsesm);
/* critical section for read only */
up_read(&mr_rwsem);
/* */

down_write(&mr_rwsem); /* writer attempts to acquire semaphore */
/* critical region for writer */
up_write(&mr_rwsem);
```

## Mutexes

- The mutex is similar to a semaphor, but only has a usage count of one.

- mutex's are represented by struct mutex.

```c
DEFINE_MUTEX(mutex_san); /* initializes mutex named mutex_san. */

// mutex_init(&mutex_san) used if you dynamically initialized mutex.

mutex_lock(&mutex);
/* critical region */
mutex_unlock(&mutex);
```

- Mutex's are simple, so try using them before semaphores
- You cannot use mutex's in interrupt context, since they go to sleep.

## Completion Variables

- Let's say we have an array, one thread fills the array, the other removes data from the array. 
    - We need to somehow coordinate it.
- This is done with the completion type defined in `<linux/completion.h>`


```c
struct completion;
init_completion(&completion); /* initializes completion struct */
wait_for_completion(&completion); /* waits for other thread to send completion signal */
complete(&completion); /* sends signal to waiting threads to wake up. */
```

## Sequential Locks

- these locks provide a mechanism for reading/writing shared data.
- A sequential lock will keep track of a number.
    - Every time a writer obtains a lock, it increments the counter. 
    - When the writer releases the lock, it increments the counter again.
    - The reader will read the data if the number on the lock is even (if it's odd, it means the writer owns the lock.)
        - Once it reads the data, it checks the number again. If it becomes odd, read the data again.

```c
seqlock_t mr_seq_lock = DEFINE_SEQLOCK(mr_seq_lock); //macro defines a seq lock.

write_seqlock(&mr_seq_lock);
/* write lock is obtained */
write_sequnlock(&mr_seq_lock);


/* reader */

unsigned long seq;

do{
    seq = read_seqbegin(&mr_seq_lock);
    /* read data here */
}while(read_seqretry(&mr_seq_lock, seq));
```

- seq locks provide a lightweight and scalable lock for use with many readers and few writers.
    - However, writers are favored over readers.

## Controlling kernel preemptions

- You can enable and disable preemptions using a couple methods:

```c
preempt_disable(); //disables kernel preemptions
preempt_enable(); //enables kernel preemptions
preempt_enable_no_sched(); //enable preemption, but doesn't cehck for any pending reschedule
preempt_count(); //return preemption count.
```

## Ordering and Barriers

- When it comes to synchronization between multiple processors or hardware devices, you'll have to sometimes explicitly order memory read and writes in your program.
    - Unfortunately, the compiler can decide to reorder these read and writes for performance reasons.
- Processors that reorder reads/writes provide instructions to enforce ordering requirements.
    - These instructions are called barriers.

- The main ones include:
    - `rmb()`: read memory barrier
    - `wmb()`: write memory barrier
    - `mb()`: read/write memory barrier

```c
static int __init mb_test_init(void)
{
    printk(KERN_INFO "Module loaded\n");

    // Writer side
    data = 123;
    wmb();       // Ensure 'data' is visible before 'flag'
    flag = 1;

    // Reader side
    while (flag == 0) cpu_relax();  // avoid busy wait

    rmb();            // Ensure we re-read 'data' after reading 'flag'
    printk(KERN_INFO "Read data = %d\n", data);

    return 0;
}
```

