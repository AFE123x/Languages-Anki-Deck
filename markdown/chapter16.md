# Chapter 16

## What is multi-threading?

- Most operating will run and manage multiple processes.
    - Processes will have their own addrees space.
- Programs can have multiple threads, to run things concurrently
    - They'll share the same address space.

## What are potential issues with multi-threading?

- Race Conditions:
    - Threads accessing data or resources in an inconsistent order
- Deadlock:
    - Two threads waiting for eachother, preventing both threads from continuing.
- Difficult to debug:
    - It can be difficult to reproduce and fix problems.


## How to create new threads?

```rs
use std::thread;
use std::time::Duration;

fn main() {
    thread::spawn(|| {
        for i in 1..10 {
            println!("hi number {i} from the spawned thread!");
            thread::sleep(Duration::from_millis(1));
        }
    });

    for i in 1..5 {
        println!("hi number {i} from the main thread!");
        thread::sleep(Duration::from_millis(1));
    }
}
```

- You can create new threads using ```thread::spawn(closure);```
    - You pass in a closure
- This will run your code simulatneously.


## What is a potential flaw with this code?

```rs
use std::thread;
use std::time::Duration;

fn main() {
    thread::spawn(|| {
        for i in 1..10 {
            println!("hi number {i} from the spawned thread!");
            thread::sleep(Duration::from_millis(1));
        }
    });

    for i in 1..5 {
        println!("hi number {i} from the main thread!");
        thread::sleep(Duration::from_millis(1));
    }
}
```

%

- The problem lies with the fact that the main thread finishes before the spawned thread. 
    - This means that the program terminates, hence killing the thread.
- We fix this using the handle.join function


## Joining threads

We can join threads using the .join() function:

```rs
use std::thread;
use std::time::Duration;

fn main() {
    let handle = thread::spawn(|| {
        for i in 1..10 {
            println!("hi number {i} from the spawned thread!");
            thread::sleep(Duration::from_millis(1));
        }
    });

    handle.join().unwrap();

    for i in 1..5 {
        println!("hi number {i} from the main thread!");
        thread::sleep(Duration::from_millis(1));
    }
}
```

## Closures - `move` Keyword

- The move keyword is a closure takes ownerships of variables used.

```rs
fn main() {
    let s = String::from("Hello");

    let closure = move || println!("{}", s); // Takes ownership of `s`
    
    closure(); 
    // println!("{}", s); // Error: value borrowed after move
}

```
- Here, the closure takes s, so we cannot use it after 

## What's wrong with this code?

```rs
use std::thread;

fn main() {
    let v = vec![1, 2, 3];

    let handle = thread::spawn(|| {
        println!("Here's a vector: {v:?}");
    });

    handle.join().unwrap();
}
```

%

- The spawned thread doesn't own v, so we need to use the move keyword here.

```rs
use std::thread;

fn main() {
    let v = vec![1, 2, 3];

    let handle = thread::spawn(move || {
        println!("Here's a vector: {v:?}");
    });

    handle.join().unwrap();
}
```

## Message passing - threads

- Instead of using memory to communicate with threads, we share memory by communicating.
- Rust implements channels, which is a programming concept where data is sent from one thread to another thread.

## Channels - Multithreading

- Channels are how rust threads communicate with each other. 
- Channels have two halves:
    - receiver: receives data.
    - transmitter: transmit data.
- A channel is closed when either the transmitter or receiver is dropped.

## Initializing Channel

```rs
use std::sync::mpsc;

fn main() {
    let (tx, rx) = mpsc::channel();
}
```
- you create a new channel using the ```mpsc::channel``` function.
    - ```mpsc``` stands for multiple producer, single consumer.

- ```mpsc``` returns a tuple consisting of a transmitter (tx), and the receiver (rx).

## How to use channels to send and receive?

```rs
use std::sync::mpsc;
use std::thread;

fn main() {
    let (tx, rx) = mpsc::channel();

    thread::spawn(move || {
        let val = String::from("hi");
        tx.send(val).unwrap();
    });

    let received = rx.recv().unwrap();
    println!("Got: {received}");
}
```

- send will send a value
- recv receives the value

## How can you send multiple messages via channels - multithreading?

```rs
use std::sync::mpsc;
use std::thread;
use std::time::Duration;

fn main() {
    let (tx, rx) = mpsc::channel();

    thread::spawn(move || {
        let vals = vec![
            String::from("hi"),
            String::from("from"),
            String::from("the"),
            String::from("thread"),
        ];

        for val in vals {
            tx.send(val).unwrap();
            thread::sleep(Duration::from_secs(1));
        }
    });

    for received in rx { //you can use for each
        println!("Got: {received}");
    }
}
```

## Having multiple transmitter for channel:

- Obviously, mpsc stands for **multiple producers, single consumer**, so we should be able to create multiple producers. 

- This is done with ```clone()``` to clone the transmitter channel:

```rs
    let (tx, rx) = mpsc::channel();

    let tx1 = tx.clone();
    thread::spawn(move || {
        let vals = vec![
            String::from("hi"),
            String::from("from"),
            String::from("the"),
            String::from("thread"),
        ];

        for val in vals {
            tx1.send(val).unwrap();
            thread::sleep(Duration::from_secs(1));
        }
    });
    thread::spawn(move || {
        let vals = vec![
            String::from("more"),
            String::from("messages"),
            String::from("for"),
            String::from("you"),
        ];

        for val in vals {
            tx.send(val).unwrap();
            thread::sleep(Duration::from_secs(1));
        }
    });
    for received in rx {
        println!("Got: {received}");
    }
```

## Shared state concurrency

- Message passing does work for multithreading, but there's another approach, communicating by sharing memory.

- Channeling is similar to single ownership, since you transfer a value down a channel.
- Sharing memory concurrency is similar to multiple owners.

- You cannot use the Rc or RefCell type in multithreaded programs. You need to use a common concurrency primitive, such as the mutex.

## Mutexes

- Mutex is an abbreviation for mutual exclusion, which ensures a mutex only accesses one thread to acess some data at some time.
- To get data in a mutex, a thread signals that it wants access to ask for the lock.
- AKA, a mutex lock guards the data it holds via a locking system.

- Mutex steps:
    - Acquire the lock before acquiring the data
    - Unlock the lock so other threads can use the lock.


## Using Mutex API

```rs
use std::sync::Mutex;

fn main() {
    let m = Mutex::new(5);

    {
        let mut num = m.lock().unwrap();
        *num = 6;
    }

    println!("m = {m:?}");
}
```

- ```lock()``` will try to acquire the lock
    - If the thread possesing the lock panicked, we wouldn't be able to reacquire the lock
- ```unwrap()``` will cause the thread to panic if the thread possessing the lock panics.

## What's wrong with the following code:

```rs
use std::sync::Mutex;
use std::thread;

fn main() {
    let counter = Mutex::new(0);
    let mut handles = vec![];

    for _ in 0..10 {
        let handle = thread::spawn(move || {
            let mut num = counter.lock().unwrap();

            *num += 1;
        });
        handles.push(handle);
    }

    for handle in handles {
        handle.join().unwrap();
    }

    println!("Result: {}", *counter.lock().unwrap());
}
```

%

- You're trying to use ```Mutex<T>``` between multiple threads.
- You'd need to somehow share the Mutex between multiple owners, similar to what Rc does.
- AKA, you'd use the Arc type!


## `Arc<T>` type

```rs
use std::sync::{Arc, Mutex};
use std::thread;

fn main() {
    let counter = Arc::new(Mutex::new(0));
    let mut handles = vec![];

    for _ in 0..10 {
        let counter = Arc::clone(&counter);
        let handle = thread::spawn(move || {
            let mut num = counter.lock().unwrap();

            *num += 1;
        });
        handles.push(handle);
    }

    for handle in handles {
        handle.join().unwrap();
    }

    println!("Result: {}", *counter.lock().unwrap());
}
```
- ```Arc<T>``` is very similar to the ```Rc<T>``` type. 
