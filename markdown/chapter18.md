# Chapter 18

## What are Patterns?

- Patterns are a special syntax in Rust for matching against the structure of types, both complex and simple.
- Being able to use patterns with match gives you move control in the program's control flow.

- Patterns consist of the combination of:
    - literals
    - destructured arrays, enums, structs or tuples
    - variables
    - wildcards
    - placeholders

## Where do we see patterns being used?

- match statements
```rs
match x {
    None => None,
    Some(i) => Some(i + 1),
}
```

- if let statements
```rs
fn main() {
    let favorite_color: Option<&str> = None;
    let is_tuesday = false;
    let age: Result<u8, _> = "34".parse();

    if let Some(color) = favorite_color {
        println!("Using your favorite color, {color}, as the background");
    } else if is_tuesday {
        println!("Tuesday is green day!");
    } else {
        println!("Using blue as the background color");
    }
}
```

- while let
```rs
    let mut stack = Vec::new();

    stack.push(1);
    stack.push(2);
    stack.push(3);

    while let Some(top) = stack.pop() {
        println!("{top}");
    }
```

- for loops
```rs
    let v = vec!['a', 'b', 'c'];

    for (index, value) in v.iter().enumerate() {
        println!("{value} is at index {index}");
    }

```

- let statements
```rs
    let (x, y, z) = (1, 2, 3);
```

- function parameters
```rs
fn print_coordinates(&(x, y): &(i32, i32)) {
    println!("Current location: ({x}, {y})");
}

fn main() {
    let point = (3, 5);
    print_coordinates(&point);
}
```

## Refutable vs Irrefutable patterns

- Irrefutable patterns will match for any value. 
    - for example, `let x = 5;`. Here, it doesn't matter what the value is, it's good
- Refutable patterns will fail to match for some values:
    - For example `let Some(x) = None`. This pattern fails, because None is not a Some.

## Matching Literals vs. variables

- this code uses literals
```rs
let x = 1;

match x {
    1 => println!("one"),
    2 => println!("two"),
    3 => println!("three"),
    _ => println!("anything"),
}

```

- This code uses variables:

```rs
    let x = Some(5);
    let y = 10;

    match x {
        Some(50) => println!("Got 50"),
        Some(y) => println!("Matched, y = {y}"), //y's in a new scope, isn't 10.
        _ => println!("Default case, x = {x:?}"),
    }

    println!("at the end: x = {x:?}, y = {y}");

```

## Match expression - muliple patterns, range of values

```rs
let x = 1;

match x {
    1 | 2 => println!("one or two"), //if x is 1 or 2
    3 => println!("three"),
    _ => println!("anything"),
}
```
- this will match with the first one if x is 1 or 2

```rs
    let x = 5;

    match x {
        1..=5 => println!("one through five"),
        _ => println!("something else"),
    }

```
- This will match the first one if it ranges from 1 to 5.

## Destructuring structures

```rs
struct Point {
    x: i32,
    y: i32,
}

fn main() {
    let p = Point { x: 0, y: 7 };

    let Point { x, y } = p; //will store struct elements in variables x and y.
    assert_eq!(0, x);
    assert_eq!(7, y);
}
```

## Destructuring Enums

```rs
enum Message {
    Quit,
    Move { x: i32, y: i32 },
    Write(String),
    ChangeColor(i32, i32, i32),
}

fn main() {
    let msg = Message::ChangeColor(0, 160, 255);

    match msg {
        Message::Quit => {
            println!("The Quit variant has no data to destructure.");
        }
        Message::Move { x, y } => {
            println!("Move in the x direction {x} and in the y direction {y}");
        }
        Message::Write(text) => {
            println!("Text message: {text}");
        }
        Message::ChangeColor(r, g, b) => { //unwraps unem and stores in variables.
            println!("Change the color to red {r}, green {g}, and blue {b}")
        }
    }
}
```


## Ignoring Values in a Pattern

- We can ignore patterns using ``_`` or ``..``

```rs
    let numbers = (2, 4, 8, 16, 32);

    match numbers {
        (first, _, third, _, fifth) => {
            println!("Some numbers: {first}, {third}, {fifth}")
        }
    }

```

```rs
    struct Point {
        x: i32,
        y: i32,
        z: i32,
    }

    let origin = Point { x: 0, y: 0, z: 0 };

    match origin {
        Point { x, .. } => println!("x is {x}"),
    }

```
- ```..``` has to have some sort of definite meaning.
    - You can't do this for example: `(.., second, ..)`


## Match Guard

```rs
    let x = 4;
    let y = false;

    match x {
        4 | 5 | 6 if y => println!("yes"),
        _ => println!("no"), //captures all other conditions.
    }

```
