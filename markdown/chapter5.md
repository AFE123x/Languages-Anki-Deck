# Chapter 5

## What is a structure?

A structure is a custom data type that lets you pack together and name multiple related values.

## How does a structure differ from a tuple?

In a structure, you can name the elements, and ordering doesn't matter.

## How do you define a structure? How do you initialize a struct type?

```rs
struct User {
    active: bool,
    username: String,
    email: String,
    sign_in_count: u64,
}

fn main() {
    let user1 = User {
        active: true,
        username: String::from("someusername123"),
        email: String::from("someone@example.com"),
        sign_in_count: 1,
    };
}
```

## We want to modify the following struct, which uses user1's struct, only with a different email. modify this struct to use shorthand:

```rs
fn main() {
    // --snip--

    let user2 = User {
        active: user1.active,
        username: user1.username,
        email: String::from("another@example.com"),
        sign_in_count: user1.sign_in_count,
    };
}
```

```rs
fn main() {
    // --snip--

    let user2 = User {
        email: String::from("another@example.com"),
        ..user1
    };
}
```

## Tuple structs

- We can specify tuple structs, so we don't need to name the field.

```rs
struct Color(i32, i32, i32);
struct Point(i32, i32, i32);

fn main() {
    let black = Color(0, 0, 0);
    let origin = Point(0, 0, 0);
}
```

## Structs and references

- To use a reference in a struct, you must specify the lifetime

```rs
struct User {
    active: bool,
    username: &str,
    email: &str,
    sign_in_count: u64,
}
```

- This struct is problematic, because we don't have a specified lifetime. We don't want username to be deallocated before email, for example.
- To fix, we need to explicitly use lifetimes

```rs
struct User<'a> {
    active: bool,
    username: &'a str,
    email: &'a str,
    sign_in_count: u64,
}
```

- Adding the lifetime will guarantee that username and email will stay valid as long as User exists.

## How can you print structures?

- To print a structure, you specify the ```#[derive(Debug)]``` above the struct.

```rs
#[derive(Debug)]
struct Rectangle {
    width: u32,
    height: u32,
}

fn main() {
    let rect1 = Rectangle {
        width: 30,
        height: 50,
    };

    println!("rect1 is {rect1:?}");
}
```

## Defining methods in structs

```rs
struct Rectangle {
    width: u32,
    height: u32,  
}

impl Rectangle{
    fn area(&self) -> u32 {
        self.width * self.height;
    }
}
fn main() {
let rect1 = Rectangle {
    width = 30,
    height = 50,
}
let x = rect1.area();
...
}
```

## Associated functions

- These functions are associated with the struct type, but not binded with an instance. ```String::from()``` is an associated function.

```rs
impl Rectangle {
    fn square(size: u32) -> Self {
        Self {
            width: size,
            height: size,
        }
    }
}
```