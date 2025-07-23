# 3. Kernel Facilities and Helper Functions

## `container_of` macro

- the `container_of` macro, given a struct, will let you get the parent struct of a certain data type.

```c
struct family {
    struct person* father;
    struct person* mother;
    int number_of_sons;
    int family_id
};

// ...
int main(){
    struct family f;
    /* initialize elements*/
    struct int* id = f.family_id; /* pointer to father element*/

    struct family* ptr = (id,struct family, family_id); 
    assert(f == ptr); // will be true.
}
```

## Linux - Linked list API introduction

- the kernel provides a linked list api in the `linux/list.h` header file.

- There's a simple `list_head` structure:

```c
struct list_head{
    struct list_head *next, *prev;
};
```

- you can include this item in any struct you want:

```c
struct car{
    int door_number;
    char* color;
    char* model;
    struct list_head list; /* kernel's list structure */
}
```

- now, you can initialize and use it:

```c
static LIST_HEAD(carlist); // create a list_head instance named carlist

struct car* redcar = kmalloc(sizeof(*car), GFP_KERNEL);

INIT_LIST_HEAD(&redcar->list); //initialize the list in the struct

list_add(&redcar->list, &carlist); //add our car to the carlist.
```



## linux linked list, dynamic and static approach to initializing

```c
struct list_head mylist;


int main(void){
    INIT_LIST_HEAD(&mylist); //initialize linked list.
}
```

- Above is the dynamic approach.

```c
LIST_HEAD(mylist) //macro
```
- this is the static approach, it creates a list_head named mylist.

## linux linked list, creating and adding list node

- to create the list, you need to define the structure, with the list_head in it.

```c
struct car{
    int door_number;
    char* color;
    char* model;
    struct list_head list; /* kernel's list structure */
}
```

- you can then kmalloc the car struct, then use the INIT_LIST_HEAD() function to initialize the list

```c
struct car* blackcar = kzalloc(sizeof(struct car), GFP_KERNEL);

INIT_LIST_HEAD(&blackcar->list); //pass in reference to list
```

- you can then add the list to another list

```c
list_add(&redcar->list, &carlist); //carlist is a global list of all cars.
```

## linux linked list - deleting node

- you can delete a node using the list_del() function:

```c
// [...]
list_del(&redcar->list); //removes redcar from the linked list
```
- you need to free the structure manually.

## linux linked list - linked list traversal

- you can use the `list_for_each_etry` to iterate through the linked list:

```c
list_for_each_entry(pos, head, member)
```
- head is the head node
- member is the member of the struct that's the list.
- pos is one of the entries of the linked list we want to start at


```c
struct car *acar; /* loop counter */
int blue_car_num = 0;
/* 'list' is the name of the list_head struct in our data structure */
list_for_each_entry(acar, carlist, list){
    if(acar->color == "blue")
    blue_car_num++;
}
```

## wait queues

- if a process is waiting for a resource, it's best to put it to sleep so the cpu can focus on more important tasks.
- a wait queue is used to process blocked I/O and wait until a particular condition is met.

## wait queue - API (declaring queue, waiting, waking up)

```c
/* statically declare wait queue */
DECLARE_WAIT_QUEUE_HEAD(name);

/* dynamically declares wait queue */
wait_queue_head_t my_wait_queue;
init_waitqueue_head(&my_wait_queue);

/* blocking process */
int wait_event_interruptible(&my_wait_queue, CONDITION); //will put process to sleep if CONDITION is false;

void wake_up_interruptable(&my_wait_queue); //will run task if CONDITION from before is true. if not, back to sleep.

void wake_up_interruptable_all(&my_wait_queue); //wakes up all tasks in the wait queue.
```

## Delay and Timer Management - types of timers

- there are two main types of timers:
    - for absolute time, there's a **real time clock (RTC)**.
    - for relative time, there's hardware on the cpu known as a **timer**. the kernel uses this for scheduling.

- We call the CPU timer as the **kernel timer**.

## kernel timer - two parts:

- We classify the kernel timer into two parts:
    - **standard timers**, or system timers.
    - high resolution timers


## standard timer - Jiffies

- the standard unit of time on standard timers is the **jiffy**.

- the jiffy increments at some time interval, determined by the constant, HZ.
    - HZ defines the number of times jiffy is incremented in a secod.

- each jiffy increment is a **tick**

- when HZ is configured, a **programmable interrupt timer (PIT), is called to increment the jiffy.

## Linux - timer struct

- the timer is included in the `linux/timer.h` header file.
- We represent the timer with the `timer_list` structure.

```c
#include<linux/timer.h>

struct timer_list{
    struct list_head entry;
    unsigned long expires;
    struct tvec_t_base_s* base;
    void (*function) (unsigned long);
    unsigned long data;
}

```

## linux - `timer_list` api (init, setting time, stopping timer)

- to initialize the timer, you can either use `setup_timer` or `init_timer`

```c
void setup_timer(struct timer_list* timer, void (*function)(unsigned long), unsigned long data);
void init_timer(struct timer_list* timer);
```

- to set the expiration time, you use `mod_timer`. it's expressed in jiffies

```c
int mod_timer(struct timer_list* timer, unsigned long expires);
```

- to release the timer, you use one of two functions:

```c
void del_timer(struct timer_list* timer); //will deactivate regardless if the handler's running
void del_timer_synce(struct timer_list* timer) //will deactivate timer once handler is completed.
```

- to check if the timer is running you can use `timer_pending`

```c
int timer_pending(const struct timer_list* timer);
```

[example](https://github.com/PacktPublishing/Linux-Device-Drivers-Development/blob/master/Chapter03/std-timer.c)

## High Resolution timers - intro and structure

- the standard timer, using jiffies, only has a resolution of milliseconds.
- HRT, in contrast, has a higher resolution in microseconds, even going up to nano seconds.


- to use HRT, you need to have the `linux/hrtimer.h` header file.

```c
struct hrtimer{
    struct timerqueue_node node;
    ktime_t _softexpires;
    enum hrtimer_restart(*function)(struct hrtimer *);
    struct hrtimer_clock_base* base;
    u8 state;
    u8 is_rel;
}
```

- the HRT is represented with the hrtimer struct


## High Resolution timers - API (init, starting, cancelling)

```c
struct hrtimer{
    struct timerqueue_node node;
    ktime_t _softexpires;
    enum hrtimer_restart (*function)(struct hrtimer *);
    struct hrtimer_clock_base* base;
    u8 state;
    u8 is_rel;
}


/* initializing timer */
void hrtimer_init(struct hrtimer* time, clockid_t which_clock, enum hrtimer_mode mode);

/* starting hrtimer */
int hrtimer_start(struct hrtimer* timer, ktime_t time, const enum hrtimer_mode mode);

/* hrtimer cancellation */
int hrtimer_cancel(struct hrtimer* timer); //waits until callback finishes.
int hrtimer_try_to_cancel(struct hrtimer* timer); //fails if timer is active or callback's running
```

[example](https://github.com/PacktPublishing/Linux-Device-Drivers-Development/blob/master/Chapter03/hr-timer.c)

## tickless kernels

- With simple timers, we constantly interrupt the process to increment the jiffy variable.
    - this means the CPU will be active, affecting power consumption.

- tickless kernels disable the simple timer interrupts until there's a task that needs to be performed.
    - when the kernel has no tasks to run, it'll sit on an idle thread, disabling the periodic ticks.

## delay and sleep in the kernel

- there are two types of delays: atomic and nonatomic.
- these are included in the `<linux/delay>` header file.

## delay/sleep - atomic context

- in an atomic context, like an interrupt handler, you cannot put it to sleep.
- in this case, you'd use a `Xdelay` family of functions that act similarly to a spinlock.

```c
udelay(unsigned long usecs)

/* these require a more precise hardware timer.*/

ndelay(unsigned long nsecs)
mdelay(unsigned long msecs)
```

## delay/sleep - nonatomic context

- Nonatomic means the thread can sleep. The kernel provides the sleep family of functions.

```c
udelay(unsigned long usecs) //yes, it's atomic, but better to use if you need to sleep < 10 uS

usleep_range(unsigned long min, unsigned long max) //uses hrtimer, and reccomend to let this sleep for a few usecs or a few msecs (10 us - 20ms)

msleep(unsigned long msecs) //use this for larger, msec sleep intervals (10ms+)
```

## Mutex locks - API (init, unlock, lock, status)

- These are mutual exclusion locks, which puts a thread to sleep if the lock is already being used

```c
/* statically declare mutex lock */
DEFINE_MUTEX(my_mutex); //creates lock named my_mutex

/* dynamically declare mutex lock */
struct mutex my_mutex;
mutex_init(&my_mutex);

/* lock */
void mutex_lock(struct mutex* lock); //only use if locks guaranteed to be released
int mutex_lock_interruptible(struct mutex* lock); //can be woken up by any signal
int mutex_lock_killable(struct mutex* lock); //can only be woken up with kill signal

/* unlock */
void mutex_unlock(struct mutex* lock);

/* check if mutex is locked or not */
int mutex_is_locked(struct mutex* lock);

/* - attempts to acquire lock. If it doesn't, 
 * it returns 1 and acquires lock. 
 * - otherwise, it returns 0.
 */
 int mutex_trylock(struct mutex* lock);
```


## spinlocks - Intro and API (init, locking and unlocking)

- spinlocks are similar to mutex locks, but do not go to sleep.
- you should only use spinlocks if you intend to not hold it for a long time.
- when you acquire a spinlock, you disable premption.

```c
/* initialize spinlock */
void spin_lock_init(spinlock_t* spinlock);

/* lock */
void spin_lock(spinlock_t* spinlock); //naive way to lock spinlock
void spin_lock_irqsave(spinlock_t* spinlock, unsigned long flag); //disable preemption on the cpu

/* unlock */
void spin_unlock(spinlock_t* spinlock); //naive way to unlock
void spin_unlock_irqrestore(spinlock_t* spinlock, flags); //re-enable premption
```

## work deferring mechanisms

- You don't want to spend too much time in the irq handler. Instead, you could defer the additional work among 3 mechanisms:
    - SoftIRQs: executed in atomic context
    - Tasklets: Executed in an atomic context
    - Workqueues: executed in a process context.


## softirqs

- These are one type of deferring mechanism that's only used for fast processing, since it disables the scheduler.
- Ideally, you'll never deal with this.
- This interrupt is handled by the ksoftirqd thread

## tasklets - struct and API (declaring, enabling/disabling, scheduling.)

- This is another type of deferring mechanism for interrupt handlers

```c
/* structure */
struct tasklet_struct{
    struct tasklet_struct* next;
    unsigned long state;
    atomic_t count;
    void (*func)(unsigned long);
    unsigned long data;
};

/* declaring tasklet */
void tasklet_init(struct tasklet_struct* t, 
void (*func)(unsigned long), 
unsigned long data); //dynamically

DECLARE_TASKLET(tasklet_example,tasklet_function,tasklet_data); //statically, enabled by default
DECLARE_TASKLET_DISABLED(name, func, data); //statically declare tasklet, disabled by default, need to call tasklet_enabled before scheduling.

/* enabling/disabling tasklet */
void tasklet_enable(struct tasklet_struct* task);
void tasklet_disable(struct tasklet_struct* task); //wait until tasklet is terminated.
void tasklet_disable_nosync(struct tasklet_struct* task); //will return immediately, even if terminantion didn't happen


/* Scheduling tasklet */
void tasklet_schedule(struct tasklet_struct* t); //schedules task.
void tasklet_hi_schedule(struct tasklet_struct* t); //gives task high priority
```

## work queues - intro, general use

- work queue is another deferring mechanism used. 
- unlike tasklets and softirqs, work queues run only in a pre-emptible context.
    - this is ideal if you need to do I/O, hold mutexes, etc.

- generally, there'll be a shared work queue.
    - when you have work to schedule, you put it into the global work queue.

- optionally, you can make your own thread for it, and wake it up everytime the handler needs to be executed.

## work queues - shared queue, API (init, scheduling, clearing)

- if you only need to submit tasks occasionally, you can use the work queue shared by the kernel.
- since this is shared, you shouldn't sleep for a long time, since other tasks on the queue will not run until you wake up. 

- you can initialize the work queue using the `INIT_WORK` macro

```c
/* initializing work */
INIT_WORK(struct work_struct* work);

/* schedule the work on the cpu */
schedule_work(struct work_struct* work);

static inline bool schedule_delayed_work(struct delayed_work* dwork, unsigned long delay); //similar to schedule_work, but delays it.

int schedule_work_on(int cpu, struct work_struct* work); //similar, but lets you choose the specific cpu to work on.

int schedule_delayed_workon(int cpu, struct delayed_work* dwork, unsigned long delay); //similar to previous, but with e delay.

/* cancelling work */
cancel_delayed_work();

/* flushing the work queue */
void flush_scheduled_work(void);
```
[example](https://github.com/PacktPublishing/Linux-Device-Drivers-Development/blob/master/Chapter03/shared-workqueue.c)


## work queues  - dedicated queue, general steps

- here, you'll have your own kernel thread to do the work.

- the general steps are as follows:
    - declare/initialize `struct workqueue_struct`
    - create the work function
    - create the struct work_struct with the work function embedded
    - embed the work function in the work_struct


## work queues - dedicated queues, API

```c
/* workqueue_struct and work_struct variables */
struct workqueue_struct* myqueue;
struct work_struct thework;

/* create handler */
void do_work(void* data);

/* initialize work structs */
struct workqueue_struct* create_singlethread_workqueue("mywork");
struct workqueue_struct* create_workqueue("mywork");
INIT_WORK(&thework, do_work, <ptr>); //initialize the work structure

/* scheduling work */
void queue_work(struct workqueue_struct* queue, struct work_struct* thework);
void queue_delayed_work(struct workqueue_struct* queue, struct work_struct* thework, delay); //similar to queue work, but with a delay, expressed in jiffies

/* waiting for all work on the waitqueue to complete */
void flush_workqueue(struct workqueue_struct* wq); //similar to .join() in multithreading

/* cleanup */
int cancel_work_sync(struct work_struct* work);
int cancel_delayed_work_sync(struct delayed_work* dwork);;2
```

- there are many variations of functions for delayed work and other things.

## Registering/freeing interrupt handler

- you can register an callback to be run when a particular interrupt line is fired.
- This is done with `request_irq`

```c
#include<linux/interrupt.h>
int request_irq(unsigned int irq, irq_handler_t handler, unsigned long flags, const char* name, void* dev);
```

- irq is the irq number
- handler is the irq handler
- flags are a bunch of options (you can check [this](https://embetronicx.com/tutorials/linux/device-drivers/linux-device-driver-tutorial-part-13-interrupt-example-program-in-linux-kernel/))
- name is used for the kernel to identify your driver in the proc interrupt and irq folder
- dev: the thing that's passed into the handler, and can identify the device.
    - you can set it to NULL if the device isn't shared.



```c
void free_irq(unsigned int irq, void* dev);
```
- irq is the irq number you want to free.
- md is the device you want to free.

## writing interrupt handler

```c
static irqreturn_t my_irq_handler(int irq, void* dev);
```

- `irqreturn_t` has two values: `IRQ_HANDLED` and `IRQ_NONE`
- `irq` is the irq number
- `dev` is the device you specified while requesting irq


## Interrupt handlers - locks

- when you're in an interrupt handler, you're in an atomic context.
- therefore, you should only use spin locks.

- if you're accessing data both userspace code and your driver can access, you should use `spin_lock_irqsave()` and `spin_lock_irqrestore()`

## Threaded IRQs

- Unlike regular interrupt handlers, where we'd have to implement a bottom half, the kernel will execute the bottom half automatically in a dedicated kernel thread.

- instead of using `request_irq()`, you use `request_threaded_irq()`

```c
int request_threaded_irq(unsigned int irq, irq_handler_t handler, irq_handler_t thread_fn, unsigned long irqflags, const char* devname, void* dev_id);
```
- handler is the irq handler
- thread_fn is the bottom half.


- this saves you from having to create a separate bottom half.