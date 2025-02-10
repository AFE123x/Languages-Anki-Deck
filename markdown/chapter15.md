# Chapter 15

## What are smart pointers?

- Smart pointers are data structures that act like pointers with additional capabilities.
- Unlike C, where you need to free memory manually, smart pointers free memory for you.

- Smart pointers, at a technical level, are structures that implement the Deref and Drop traits.
    - Deref allows the struct to act like a reference when you work with it
    - Drop lets you customize what happens when a function goes out of scope

## What are the 3 types of smart pointers rust offers?

- ```Box<T>``` for allocating values on the heap
- ```Rc<T>```, which is a reference counting type that enables multiple ownerships.
- ```Ref<T>``` and ```RefMut<T>```, accessed through ```RefCell<T>```, which enforces borrowing rules at runtime instead of compile time.

## `Box<T>` smart point

- Boxes let you store data on the heap.

```rs
fn main(){
    let b: Box<i32> = Box::new(5);
    println!("b = {}",b);
}
```
- 5 is stored on the heap, the location of 5 is stored in b, which is stored on the stack.
- When b is out of scope, it'll be deallocated.

## What is wrong with the following?

```rs
enum List{
    Cons(i32, List),
    Nil,
}

fn main(){
    let list = Cons(1,Cons(2,Cons(3,Nil)));
}
```

%

You're allocating the List recursively, which would have an infinite size. You should put the List into the Box<List> smart pointer

```rs
enum List{
    Cons(i32,Box<List>),
    Nil,
}
fn main(){
    let list = Cons(1,Box::new(Cons(2,Box::new(3,Nil))));
}
```

- This will compile.


## Deref Trait

- The deref train defines the behavior of the ```*``` operator on a custom type
- If you implement the deref trait, you'll have to implement he ```fn deref``` trait

```rs
use std::ops::Deref;

struct MyBox<T>(T);

impl<T> Deref for MyBox<T> {
    type Target = T;

    fn deref(&self) -> &Self::Target {
        &self.0
    }
}

fn main() {
    let x = MyBox(42);
    println!("{}", *x); // Works like a regular reference!
}

```

## Deref Coercion

- For types that is a reference to another reference, and so on, rust will do the full dereference for us:

```rs
fn greet(name: &str) {
    println!("Hello, {}!", name);
}
fn main(){
    let s = MyBox(String::from("Alice"));
    greet(&s); // Works because Deref turns &MyBox<String> into &String, then &str
}
```

- Here, it'll dereference and print out ```Hello, Alice```, instead of printing the reference.

## Deref coercion with Mutability

- You can use the ```DerefMut``` trait to override the ```*``` operator on mutable references.
- There are three cases of deref coercion:
    - From ```&T``` to ```&U``` when we do ```T:Deref<Target = U>```
    - We can deference a ```&mut T``` to a ```&mut U``` when ```T: DerefMut<Target = U>```
    - We can dereference ```&mut T``` to ```&U``` when ```T: Deref<Target = U>```.

- Basically, you cannot dereference a immutable reference to a mutable reference.

## Drop trait

- This trait let's us custamize what happens when a value goes out of scope. The ```Drop``` trait requires you to implement the ```drop()``` function

```rs
struct CustomSmartPointer {
    data: String,
}

impl Drop for CustomSmartPointer {
    fn drop(&mut self) {
        println!("Dropping CustomSmartPointer with data `{}`!", self.data);
    }
}

fn main() {
    let c = CustomSmartPointer {
        data: String::from("my stuff"),
    };
    let d = CustomSmartPointer {
        data: String::from("other stuff"),
    };
    println!("CustomSmartPointers created.");
}
```

## Explicitly dropping a value

- You can explicitly drop a value using the ```drop()``` function:

```rs
fn main() {
    let c = CustomSmartPointer {
        data: String::from("some data"),
    };
    println!("CustomSmartPointer created.");
    drop(c);
    println!("CustomSmartPointer dropped before the end of main.");
}
```

## `Rc<T>`, Reference Count Smart Pointer

- Rc pointers are like box pointers, but let us share data!

```rs
enum List {
    Cons(i32, Rc<List>),
    Nil,
}

use crate::List::{Cons, Nil};
use std::rc::Rc;

fn main() {
    let a = Rc::new(Cons(5, Rc::new(Cons(10, Rc::new(Nil)))));
    let b = Cons(3, Rc::clone(&a));
    let c = Cons(4, Rc::clone(&a));
}
```
- Once there are no more references, the pointer is freed from memory.
- ```clone``` doesn't actually clone the data, it just increases the reference count.

- We can use ```Rc::strong_count(&a)``` which gives us the number of references.
    - This function gives the **strong count**.


## Interior Mutability Pattern

- Interior mutability is a design pattern in Rust that let's you mutate data with immutable references.
    - To do this, you can write unsafe code to bend the usual rules of mutation and borrowing. 

## `RefCell<T>`

- Unlike ```Rc<T>```, ```RefCell<T>```represents single ownership over the data it holds.

- This seems like the box type, but the rules about references aren't check during compile time for RefCell types.
    - RefCell types are checked during run time.
- This is typically recommended if the compiler isn't guaranteed to understand you program.


## Rc and RefCells vs. multithreading

- You can only use ```Rc<T>``` and ```RefCell<T>``` in single threaded scenarios.
- The compiler will not compile your code if you're using multithreading.

## RefCells and interior mutability

```rs
use std::cell::RefCell;

struct Immutable {
    value: RefCell<i32>,
}

fn main() {
    let data = Immutable {
        value: RefCell::new(10),
    };

    *data.value.borrow_mut() = 20;

    println!("{}", data.value.borrow());
}
```

- ```borrow()``` will return a immutable reference
- ```borrow_mut()``` will return a mutable reference.
- The program will crash if the rule is violated

## Reference Cycles

```rs
fn main() {
    let a = Rc::new(Cons(5, RefCell::new(Rc::new(Nil))));

    println!("a initial rc count = {}", Rc::strong_count(&a));
    println!("a next item = {:?}", a.tail());

    let b = Rc::new(Cons(10, RefCell::new(Rc::clone(&a))));

    println!("a rc count after b creation = {}", Rc::strong_count(&a));
    println!("b initial rc count = {}", Rc::strong_count(&b));
    println!("b next item = {:?}", b.tail());

    if let Some(link) = a.tail() { //PART A
        *link.borrow_mut() = Rc::clone(&b);
    }

    println!("b rc count after changing a = {}", Rc::strong_count(&b));
    println!("a rc count after changing a = {}", Rc::strong_count(&a));
    // println!("a next item = {:?}", a.tail());
}
```

- PART A: Here, you can see that we connect the tail of the list to b, which causes a cycle. Problem is, the reference count will never become zero.
- We use Weak pointers to deal with it.

## Weak Pointers

- To fix this, we turn Rc types into Weak type. To do this, you use ```Rc::downgrade(&strong_rc);```

```rs
use std::rc::{Rc, Weak};

fn main() {
    let strong_rc = Rc::new(42);

    // Create a Weak reference from the strong Rc
    let weak_rc: Weak<i32> = Rc::downgrade(&strong_rc);

    // Upgrade Weak to Rc if the value still exists
    if let Some(strong_again) = weak_rc.upgrade() {
        println!("Value is still alive: {}", *strong_again);
    } else {
        println!("Value has been dropped");
    }

    // Drop the strong reference
    drop(strong_rc);

    // Now, upgrading the weak pointer should fail
    if weak_rc.upgrade().is_none() {
        println!("The value has been dropped, weak Rc is now invalid.");
    }
}
```

