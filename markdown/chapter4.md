# Chapter 4

## What is ownership?

- Ownership is the set of rules that govern how a Rust program manages memory.
- Some languages have a garbage collector, while other languages have the programmer manually allocate and free memory.
- Rust has a set of rules which the compiler checks. If the rules are violated, the program won't compile.

## Stack vs. Heap memory

- Stack memory stores variables where the size is known (like arrays, ints, etc.). The only limit is that stack memory lives throughout the lifetime of the function
- Heap memory, on the other hand, stores things where the size is only known during runtime. 

## What are the ownership rules?

- Each value in rust has an owner
- There can only be one owner at a time
- When the owner goes out of scope, the value will be dropped.

## What is variable scope?

```rs
fn main(){
    let s = "Hello";
    {
        let s = "howdy";
    } //here, the s is in it's own scope, 
    println!("{}",s); //here, the s in the {} is no longer valid.
}
```

## String Type

- The String type is stored on the heap.
    - The string literals are hardcoded into the program.
        - They work fine, but they're no tsustainable to use in all cases. For one, they're immutable.
- The string type is mutable

```rs
let mut s = String::from("Hello");
s.push_str(", world!"); //will add , world! to s
println!("{}",s); //prints "Hello, world!"
```

## moving vs references

```rs
let x = 5;
let y = x;
```

- Here, we copy 5 from x and store it in y. we know the sizes of these ,so we copy them.

```rs
let s1 = String::from("Hello");
let s2 = s1;
```

- We don't know the size of s1 immediately, so when we set s1 = s2, s2 points to s1. Specifically, the memory address where the first character is located.

## Having only one owner at a time

- You can only have one owner at a time:

```rs
let s1 = String::from("Hello");
let s2 = s1;
println!("{}",s1); //this isn't allowed, since s2 owns the data, s1 lost it's ownership.
```

## Deallocating memory in Rust

- In rust, memory is freed when it goes out of scope. This is the approach instead of using a garbage collector (slow) or manually freeing (buggy).

```rs
{
    let s = String::from("hello");
} //here, s is freed, goes out of scope.
```

## Using clone on heap data

- If we want to fully clone heap allocated data, we can use the    ```clone()``` method.

```rs
let s1 = String::from("hello");
let s2 = s1.clone();
println!("s1 = {}, s2 = {}",s1,s2);
```

## Ownership and functions

- When you pass a value into a function, similar to assignments, will either move or copy the variable. 

```rs
fn main() {
    let s = String::from("hello");  // s comes into scope

    takes_ownership(s);             // s's value moves into the function...
                                    // ... and so is no longer valid here

    let x = 5;                      // x comes into scope

    makes_copy(x);                  // x would move into the function,
                                    // but i32 is Copy, so it's okay to still
                                    // use x afterward

} // Here, x goes out of scope, then s. But because s's value was moved, nothing
  // special happens.

fn takes_ownership(some_string: String) { // some_string comes into scope
    println!("{some_string}");
} // Here, some_string goes out of scope and `drop` is called. The backing
  // memory is freed.

fn makes_copy(some_integer: i32) { // some_integer comes into scope
    println!("{some_integer}");
} // Here, some_integer goes out of scope. Nothing special happens.
```

## Return values and scope

Returning a value will also transfer ownership

```rs
fn main() {
    let s1 = gives_ownership();         // gives_ownership moves its return
                                        // value into s1

    let s2 = String::from("hello");     // s2 comes into scope

    let s3 = takes_and_gives_back(s2);  // s2 is moved into
                                        // takes_and_gives_back, which also
                                        // moves its return value into s3
} // Here, s3 goes out of scope and is dropped. s2 was moved, so nothing
  // happens. s1 goes out of scope and is dropped.

fn gives_ownership() -> String {             // gives_ownership will move its
                                             // return value into the function
                                             // that calls it

    let some_string = String::from("yours"); // some_string comes into scope

    some_string                              // some_string is returned and
                                             // moves out to the calling
                                             // function
}

// This function takes a String and returns one
fn takes_and_gives_back(a_string: String) -> String { // a_string comes into
                                                      // scope

    a_string  // a_string is returned and moves out to the calling function
}
```

## References and borrowing

What if, you don't want to return a string? You can use something known as **borrowing**

```rs
fn main() {
    let s1 = String::from("hello");

    let len = calculate_length(&s1);

    println!("The length of '{s1}' is {len}.");
}

fn calculate_length(s: &String) -> usize {
    s.len()
}
```

- Here, we pass in a reference to the string instead, which will not give ownership to calculate_length

## Mutable references and borrowing

It's similar to immitable references, but you add a mut between the ``&`` and name:

```rs
fn main() {
    let mut s = String::from("hello");

    change(&mut s);
}

fn change(some_string: &mut String) {
    some_string.push_str(", world");
}
```

## Number of mutable and immutable references

- You can have infinite number of immutable references, but you cannot have immutable references with mutable references.

```rs
    let mut s = String::from("hello");

    let r1 = &s; // no problem
    let r2 = &s; // no problem
    let r3 = &mut s; // BIG PROBLEM

    println!("{}, {}, and {}", r1, r2, r3);
```

- You cannot have multiple mutable references.

```rs
    let mut s = String::from("hello");

    let r1 = &mut s;
    let r2 = &mut s;

    println!("{}, {}", r1, r2);

```

- It's okay to have a mutable reference, only if you don't use the immutable references afterwards:

```rs
    let mut s = String::from("hello");

    let r1 = &s; // no problem
    let r2 = &s; // no problem
    println!("{r1} and {r2}");
    // variables r1 and r2 will not be used after this point

    let r3 = &mut s; // no problem
    println!("{r3}");

```


## What does ``s.as_bytes().iter().enumerate()`` do?

- as_bytes will convert a string into an array of bytes.
- iter will create an iterator of bytes
- enumerate will wrap the index number and the actual byte (index, &item)

```rs
fn first_word(s: &String) -> usize {
    let bytes = s.as_bytes();

    for (i, &item) in bytes.iter().enumerate() {
        if item == b' ' {
            return i;
        }
    }

    s.len()
}
```

## string slices:

we can make references to particular parts of the array:

```rs
let s = String::from("hello world");
let hello = &s[0..5]; //will have hello
let world = &s[6..11]; //will have world
let slice = &s[..5]; //hello
let slice2 = &s[6..] // world
let slice3 = &s[...] //the whole string.
```

## Write a program that returns a string of the first word of a sentence:

```rs
fn first_word(s: &String) -> &str {
    let bytes = s.as_bytes();

    for (i, &item) in bytes.iter().enumerate() {
        if item == b' ' {
            return &s[0..i];
        }
    }

    &s[..]
}
```

## What is the problem with the following code?

```rs
fn main() {
    let mut s = String::from("hello world");

    let word = first_word(&s);

    s.clear(); // error!

    println!("the first word is: {word}");
}
```

- this code is wrong because we try to use a mutable reference vs. making an immutable borrow. To fix this, we should move s.clear() before word, or pass in a muatble reference.