# Chapter 19

## What is Unsafe Rust?

- Rust guarantees memory safety at compile time.
- Unsafe Rust doesn't enforce these memory rules.

- It basically trusts you, the programer, ta make safe decisions.
- Unsafe rust also lets you directly interact with hardware.

## What can you do with unsafe rust?

- Dereference a raw pointer.
- Call an unsafe function or method.
- access or modify a mutable static variable.
- implement an unsafe trait.
- access fields of a union.

## Making mistakes in unsafe rust

- One thing that's useful about unsafe rust is that, if you make a mistake, you know it'll be in an unsafe block.
- It's better to keep unsafe blocks at a minimum (you'll thank yourself later).


## Keeping unsafe code in APIs

- It's recommended to nest unsafe code in safe APIs, to make troubleshooting easy, and to isolate unsafe code.

## Dereferencing a raw pointer

- Unsafe rust introduces two new types known as raw pointers:
    - mutable pointers: `*mut u32`, for example.
    - immutable pointers: `*const u23`, for example.

```rs
    let mut num = 5;

    /* creating the pointers is safe*/
    let r1 = &num as *const i32;
    let r2 = &mut num as *mut i32;

    unsafe {
        /* dereferencing in the unsafe operation here.*/
        println!("r1 is: {}", *r1);
        println!("r2 is: {}", *r2);
    }

```

- This works similarly to smart pointers, except:
    - you can have both immutable and mutable pointers
    - multiple mutable pointers to the same location.
    - Aren't guaranteed to point to valid memory.
    - It can be NULL
    - Doesn't implement any automatic cleanup.

## Unsafe Functions or Methods

```rs
    unsafe fn dangerous() {}

    unsafe {
        dangerous();
    }

```

- You can only call unsafe functions in unsafe blocks, otherwise your code won't compile.
    - This tells the compiler that you know how to use the function properly.

- In rust, it's common practice to provide a safe abstraction over unsafe code.

## Creating Safe Abstractions over Unsafe code

```rs
fn split_at_mut(values: &mut [i32], mid: usize) -> (&mut [i32], &mut [i32]) {
    let len = values.len();

    assert!(mid <= len);

    (&mut values[..mid], &mut values[mid..])
}
```

- You'd think this would work, but it doesn't since mid overlaps.


```rs
use std::slice;

fn split_at_mut(values: &mut [i32], mid: usize) -> (&mut [i32], &mut [i32]) {
    let len = values.len();
    let ptr = values.as_mut_ptr();

    assert!(mid <= len);

    unsafe {
        (
            slice::from_raw_parts_mut(ptr, mid),
            slice::from_raw_parts_mut(ptr.add(mid), len - mid),
        )
    }
}

```

- here, we create a i32 pointer from an array, ptr. We then call an unsafe block to use the from_raw_parts_mut to make the slices from a pointer.

## Unsafe code: Calling external code

```rs
extern "C" {
    fn abs(input: i32) -> i32;
}

fn main() {
    unsafe {
        println!("Absolute value of -3 according to C: {}", abs(-3));
    }
}
```
- You can use a Foreign Function Interface to call functions from a different language.
- Here, we call a C implementation of abs from rust. 

## Unsafe code: Accessing or Modifying a mutable static variable

- global variables are called static variables.

- If we want to modify a static variable, we need to use the unsafe block

```rs
static mut COUNTER: u32 = 0;

fn add_to_count(inc: u32) {
    unsafe {
        COUNTER += inc;
    }
}

fn main() {
    add_to_count(3);

    unsafe {
        println!("COUNTER: {COUNTER}");
    }
}
```

## Implementing unsafe traits

```rs
unsafe trait Foo {
    // methods go here
}

unsafe impl Foo for i32 {
    // method implementations go here
}

fn main() {}
```

- A trait is unsafe if one of its methods has some invariant that the compiler can't verify.

## Unsafe code - Accessing Fields of a union

- You can use unsafe to access fields of a union.
    - This is really meant for interfacing with C code.


## Traits - Associated types

```rs
pub trait Iterator {
    type Item;

    fn next(&mut self) -> Option<Self::Item>; //Item is the placeholder we have.
}
```

- Associated types are placeholders where you can add to traits. 
    - methods can use said placeholders

## Difference between Associated types and generics

- You can only have one concrete type per implementation.
- Generics can only let you have as many.

```rs
pub trait Iterator {
    type Item;

    fn next(&mut self) -> Option<Self::Item>;
}

impl Iterator for Counter {
    type Item = u32;

    fn next(&mut self) -> Option<Self::Item> {
        // --snip--
// ...
```

- You can't have another implementation of Iterator, just for u32.


## Default Generic Type Parameters

- Generic type paramaters let us specify a default type for generic types

```rs
struct Wrapper<T = i32> {
    value: T,
}

impl<T> Wrapper<T> {
    fn new(value: T) -> Self {
        Wrapper { value }
    }
}

fn main() {
    let default_wrapper = Wrapper::new(42); // Uses the default i32
    let custom_wrapper = Wrapper::<f64>::new(3.14); // Explicitly specifies f64

    println!("Default: {}", default_wrapper.value);
    println!("Custom: {}", custom_wrapper.value);
}
```

## Operator overloading

- We can overload certain operations in `std::ops`.

```rs
use std::ops::Add;

#[derive(Debug, Copy, Clone, PartialEq)]
struct Point {
    x: i32,
    y: i32,
}

impl Add for Point {
    type Output = Point;

    fn add(self, other: Point) -> Point {
        Point {
            x: self.x + other.x,
            y: self.y + other.y,
        }
    }
}

fn main() {
    assert_eq!(
        Point { x: 1, y: 0 } + Point { x: 2, y: 3 },
        Point { x: 3, y: 3 }
    );
}
```

- In this example, we implement the Add trait for the struct struct.

## Calling Methods with same name

```rs
trait Pilot {
    fn fly(&self);
}

trait Wizard {
    fn fly(&self);
}

struct Human;

impl Pilot for Human {
    fn fly(&self) {
        println!("This is your captain speaking.");
    }
}

impl Wizard for Human {
    fn fly(&self) {
        println!("Up!");
    }
}

impl Human {
    fn fly(&self) {
        println!("*waving arms furiously*");
    }
}

```
- You specify the trait name before the function name to specify the instance of fly:

```rs
fn main() {
    let person = Human;
    Pilot::fly(&person);
    Wizard::fly(&person);
    person.fly();
}
```

## Super Traits

- Super Traits are traits that require another tarit to be implemented beforehand.

```rs
use std::fmt;

// Supertrait: Displayable requires Display to be implemented
trait Displayable: fmt::Display {
    fn display_info(&self) {
        println!("{}", self);
    }
}

// Struct implementing Display first
struct Item {
    name: String,
}

impl fmt::Display for Item {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "Item: {}", self.name)
    }
}

// Now we can implement Displayable, since Display is already implemented
impl Displayable for Item {}

fn main() {
    let item = Item { name: "RustBook".to_string() };
    item.display_info(); // Calls the method from Displayable trait
}

```
- Here, to implement Displayable, we need to implement fmt::Display

## Type synonyms

```rs
type Kilometers = i32; //rust
```
```c
typedef int Kilometers; //C
```

## Never type that never returns

- We use `!` as the return type to specify that the function or whatever never returns to the caller:

```rs
fn crash() -> ! {
    panic!("This function never returns!");
}
```
- The caller will not have control of the program when `crash()` is called.


- `panic!()` also has the `!` type since it doesn't return.

## Function Pointers

We can pass in functions into functions using function pointers:

```rs
fn add_one(x: i32) -> i32 {
    x + 1
}

fn do_twice(f: fn(i32) -> i32, arg: i32) -> i32 {
    f(arg) + f(arg)
}

fn main() {
    let answer = do_twice(add_one, 5);

    println!("The answer is: {answer}");
}
```

- `fn(i32) -> i32` tells us that we need to pass in a function that has one i32 argument, and returns an i32 type.

## returning cloures

```rs
fn returns_closure() -> Box<dyn Fn(i32) -> i32> {
    Box::new(|x| x + 1)
}
```

- We store the closure in a Box type, specifying `dyn`.
    - `dyn` means that we do not know the size of this at compile time.



## What are Macros?

- Macros allow you to write code that writes other code.
    - They're similar to functions, but without the runtime cost.
    - It does increase compile time, since the compiler expands the macros during compile time.


## What are the types of macros in Rust?

- **Declarative macros**: These enable you to write something similar to a match expression that operates on Rust code arguments. It then uses the code you provided to generate code and replace the macro invocation.
- **procedural macros**: These let you operate on the abstract syntax tree (AST) of the Rust Code it's given.
    - A proc macro is a function from a TakenStream to another TokenStream.
        - The output replaces the macro invocation.


## Declarative macros in Rust

- There macros are declared using the `macro_rules!` keyword.
- Declarative macros provide an match like interface where the macro is replaced with code inside the matched arm.

```rs
macro_rules! add{
    ($a:expr,$b:expr)=>{ //a and b are expressions,  add!(1,2);
        {
            $a+$b
        }
    };

    ($a:expr)=>{ //second match, add!(1)
        {
            $a
        }
    }
}

fn main(){
 // call to macro, $a=1 and $b=2
    add!(1,2);
}
```

## Macro token types

- item: like functions, structs, modules, etc.
- bloc: statements/expressions surrounded by braces
- stmt: A statement
- pat: A Pattern
- expr: an expression
- ty: a type
- ident: an identifier
- path: a path (foo, std::mem::replace),

## Macro token types - item

```rs
macro_rules! make_function {
    ($name:ident) => {
        fn $name() {
            println!("Function {} called!", stringify!($name));
        }
    };
}

make_function!(hello);
hello();
```

- You can pass in functions modules, structs, etc.

## Macro token types - block

```rs
macro_rules! repeat_twice {
    ($blk:block) => {
        {
            $blk
            $blk
        }
    };
}

fn main() {
    repeat_twice!({
        println!("Hello, world!");
    });
}
```

- You pass in a block of statements or expression, surrounded by braces.

## Macro token types - stmt
```rs
macro_rules! make_statement {
    ($s:stmt) => {
        {
            println!("Before statement");
            $s
            println!("After statement");
        }
    };
}

fn main() {
    make_statement!(let x = 42;);
}
```
- Here, we pass in statements
    - Vairable declarations, expression statements, functions that don't return a value.


## Macro token types - pat

```rs
macro_rules! match_pattern {
    ($p:pat) => {
        match 42 {
            $p => println!("Matched!"),
            _ => println!("No match"),
        }
    };
}

fn main() {
    match_pattern!(42);
}
```

- this is for pattern matching

## Macro token types - expr

```rs
macro_rules! eval_expr {
    ($e:expr) => {
        println!("Result: {}", $e);
    };
}

fn main() {
    eval_expr!(3 + 4);
}
```

- We pass in an expression

## Macro token types - ty

```rs
macro_rules! define_struct {
    ($t:ty) => {
        struct Container {
            value: $t,
        }
    };
}

define_struct!(i32);

fn main() {
    let _c = Container { value: 10 };
}
```

- Here, we pass in a data type. Here, we pass in i32.

## Macro Token types - ident

```rs
macro_rules! create_variable {
    ($name:ident) => {
        let $name = 42;
    };
}

fn main() {
    create_variable!(x);
    println!("{}", x);
}
```

- An ident is the identity of a variable name, function, things like that.

## Macro Token types - path

```rs
macro_rules! call_function {
    ($path:path) => {
        $path();
    };
}

fn hello() {
    println!("Hello, world!");
}

fn main() {
    call_function!(hello);
}
```
- Here, we pass in a path to a function.

## Macro Token types - tt

```rs
macro_rules! wrap_in_braces {
    ($t:tt) => {
        {
            println!("Braced:");
            println!("{:?}", stringify!($t));
        }
    };
}

fn main() {
    wrap_in_braces!(42 + 3);
}
```
- This denotes any sequence of tokens.

## Macro Token types - vis

```rs
macro_rules! define_struct {
    ($vis:vis) => {
        $vis struct PublicStruct {
            value: i32,
        }
    };
}

define_struct!(pub);

fn main() {
    let _s = PublicStruct { value: 10 };
}
```
- Here, we pass in a visibility keyword: Public or Private

https://blog.logrocket.com/macros-in-rust-a-tutorial-with-examples/#whatarerustmacros
