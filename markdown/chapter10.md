# Chapter 10

## What are generics

```rs
fn largest_i32<T: PartialOrd + Copy>(list: &[T]) -> &T {
    let mut largest = &list[0];

    for item in list {
        if item > largest {
            largest = item;
        }
    }

    largest
}

fn main() {
    let number_list = vec![34, 50, 25, 100, 65];

    let result = largest_i32(&number_list);
    println!("The largest number is {result}");
    let char_list = vec!['y', 'm', 'a', 'q'];
    let result = largest_char(&char_list);
    println!("The largest char is {result}");
}
```
- Repetitive code sucks, we can use generics to use a single function to work with multiple data types.
- We define <T> after the function name, and replace the types with T.
- We specify ```PartialOrd + Copy```, as T should support these two.

## What is the problem with this structure?

```rs
Struct Point<T>{
    x: T,
    y: T,
}

fn main(){
    let p1 = Point {
        x: 5,
        y: 10.0,
    };
}

```

- We're specifying x and y as different types despite the struct only expected one type. We can change the Struct, or change y to 10.


## How would you write the impl block for this struct:

```rs
struct point<T> {
    x: T,
    y: T,
}
```

```rs
impl<U> Point<U> {
    fn x(&self) -> &U {
        &self.x
    }
}
```

## Let's say we have the following struct, what does the f32 block tell us?

```rs
struct Point<T>{
    x: T,
    y: T,
}

impl point<f64> {
    y(&self) -> f64 {
        self.y
    }
}
```

- This tells us that the function y is available **only if** we initialize the Point to use f64 type. Otherwise, it won't work.

## Generics in functions

```rs
impl<T, U> Point <T, U> {
    fn mixup<V,W>(self, other: Point<V,W>) -> Point<T,W> {
        Point {
            x: self.x, //gets T from self
            y: other.y, //gets W from point.
        }
    }
}
```

- We specify the generic types after the function name, then use them in the arguments and return type.
- We can mixup generics from different types, like here.

## Traits

```rs
// Define a trait named `Speak`
trait Speak {
    fn speak(&self);
}

// Implement the `Speak` trait for a struct `Dog`
struct Dog;

impl Speak for Dog {
    fn speak(&self) {
        println!("Woof!");
    }
}

// Implement the `Speak` trait for a struct `Cat`
struct Cat;

impl Speak for Cat {
    fn speak(&self) {
        println!("Meow!");
    }
}

fn main() {
    let dog = Dog;
    let cat = Cat;

    dog.speak();
    cat.speak();
}
```

- A trait lets you define shared behavior accross multiple types.
- In this example, we define a trait, Speak, and we implement them for dogs and cats.

## Trait Bounds

- A trait bound is the use require the argument to implement the trait:

```rs
// Define a trait
trait Speak {
    fn speak(&self);
}
trait Sniff {
    fn sniffy(&self);
}
// Implement the trait for two types
struct Dog;
struct Cat;

impl Speak for Dog {
    fn speak(&self) {
        println!("Woof!");
    }
}
impl Sniff for Dog {
    fn sniffy(&self) {
        println!("sniff sniff");
    }
}

impl Sniff for Cat {
    fn sniffy(&self){
        println!("sniff dumb cat");
    }
}
impl Speak for Cat {
    fn speak(&self) {
        println!("Meow!");
    }
}

fn make_it_speak(animal: &impl Speak) { // requires type to implement Speak.
    animal.speak();
}

fn make_it_speak2<T: Speak + Sniff>(item: &T){
    animal.speak();
}

fn main() {
    let dog = Dog;
    let cat = Cat;

    make_it_speak(&dog); // Outputs: Woof!
    make_it_speak(&cat); // Outputs: Meow!
}

```

- make_it_speak requires animal, the argument, to implement Speak trait.
- Similar to the ```implements``` keyword in java.
- We can require multiple types using +


## Where keyword

```rs
fn some_function<T: Display + Clone, U: Clone + Debug>(t: &T, u: &U) -> i32 {

}
```
- This is pretty cumbersome (says no one), so we can use the where clause.

```rs
fn some_function<T, U>(t: &T, u: &U) -> i32
    where T: Display + Clone,
          U: Clone + Debug
{
    //...
}
```

## Returning types that implement a trait

- You can return a type that implements a trait:

```rs
fn returns_summarizable() -> impl Summary {
    Tweet {
        username: String::from("horse_ebook"),
        content: String::from("of course, as you probably already know, people"),
        reply: false,
        retweet: false,
    }
}

fn main(){
    println!("{}",returns_summarizable().summarize());
}
```

- This says that you can only return data types that implement the Summary trait.
- We can safely call summarize here, because we know the return type implements the Summary trait.

## Conditionally Implement Methods

- You can implement methods based on the traits a generic has:

```rs
use std::fmt::Display;

struct Pair<T> {
    x: T,
    y: T,
}

impl<T> Pair<T> {
    fn new(x: T, y: T) -> Self {
        self {x,y}
    }
}

impl<T: Display + PartialOrd> Pair<T> {
    fn cmp_display(&self) {
        if self.x >= self.y {
            println!("The largest member is x = {}",self.x);
        }
        else {
            println!("The largest member is y = {}",self.y);
        }
    }
}
```

- This says that we can call cmp_display only if the type, T, implements the Display and PratialOrd. new works regardless of the type

## What is a lifetime

```rs
fn main() {
    let r;                // ---------+-- 'a
                          //          |
    {                     //          |
        let x = 5;        // -+-- 'b  |
        r = &x;           //  |       |
    }                     // -+       |
                          //          |
    println!("r: {r}");   //          |
}                         // ---------+
```

- The lifetime specifies how long a variable lasts.
- ```'b``` has a shorter lifetime than ```'a```, so r would have a dangling pointer, which the borrow checker would find.


## Does this code compile?

```rs
fn longest(x: &str, y: &str) -> &str {
    if x.len() > y.len() {
        x
    } else {
        y
    }
}
```

- This code doesn't compile, because x and y may have different lifetimes.
    - we're returning the reference to it, which may cause memory bugs due to the different lifetime. 
- To fix this, we need to explicitly give the lifetime.

```rs
fn longest<'a>(x: &'a str, y: &'a str) -> &'a str {
    if x.len() > y.len() {
        x
    } else {
        y
    }
}
```
- Here, we explicitly state the lifetime, so we tell the borrow checker that x and y should have the same lifetime.
- The lifetime of the return value will be the lifetime of the item with the smallest lifetime

```rs
let string1 = String::from("abcd");
let result;
{                                                       //------+
    let string2 = String::from("xyz");                  //      |result lifetime
    result = longest(string1.as_str(), string2.as_str();//      |
}                                                       //------+
    println!("The longest string is {}",result);
```
- In this example, result wouldn't be valid, because the lifetime of result is the same as the lifetime of string2, which only lasts in the inner scope.


## Lifetimes in structs

- We need to specify lifetimes if we use a reference:

```rs
struct ImportantExcerpt<'a>{
    part: &'a str,
    part2: &'a str,
}

```

## Lifetime elision

- Sometimes, we don't need to explicitly state the lifetimes. Instead, the borrow check can infer the lifetimes, given these three ellisions:
    - Each parameter that's a refrence gets its own lifetime parameter.
    - If there's one input lifetime parameter, the lifetime is assigned to all output lifetime parameters.
    - If there are multiple input lifetime parameters, but one of them is &self, or &mut self, the lifetime of self is assigned to all lifetime parameters.

```rs
fn first_word(s: &str) -> &str {
    let bytes = s.as_bytes();

    for (i, &item) in bytes.iter().enumerate() {
        if item == b' ' {
            return &s[0..i];
        }
    }

    &s[..]
}
```

- In this example, we don't need to specify the lifetime, because there's only one argument, is the return value has the same lifetime as s.


## Static lifetimes

```rs
let s: &'static str = "HELLO!!!";
```

- Static lifetimes live throughout the lifetime of the program
