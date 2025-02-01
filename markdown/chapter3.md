## What are immutable vs mutable data types

- immutable data types cannot be modified
    - by default, data types are immutable
- mutable types can be modified.

```rs
let x = 5;
let mut x = 5; //this can be modified later
```

## Will this code compile?

```rs
fn main(){
    let x = 5;
    println!("{}",x);
    x = 7;
}
```

% 

This code will not compile, because you're trying to modify an immutable type.


## Constants in rust

- These types cannot be mutable, they're always immutable

```rs
const THREE_HOURS_IN_SECONDS: u32 = 60 * 60 * 3;
```

## Shadowing

- Instead of naming multiple different variables, you can declare variables with the same name:

```rs
fn main() {
    let x = 5;

    let x = x + 1;

    {
        let x = x * 2;
        println!("The value of x in the inner scope is: {x}");
    }

    println!("The value of x is: {x}");
}
```

## Scalar Types

- Rust has four Scalar types:
    - Integers
    - Floating point numbers
    - Booleans
    - Characters

## Integer Types

| length | signed | unsigned |
| --- | --- | --- |
| 8 bits | i8 | u8 |
| 16 bits | i16 | u16 |
| 32 bits | i32 | u32 |
| 64 bits | i64 | u64 |
| 128 bits | i128 | u128 |
| architecture | isize | usize |

## Integer Literals

| Number literals | example |
| --- | --- |
| Decimal | ```98_222``` |
| Hex | ```0xff``` |
| Octal | ```0o77``` |
| Binary | ```0b1111_0000``` |
| Byte (u8 only) | ```b'A'```|

## Handling Integer Overflow

- There are four ways to handle overflow:
    - wrapping_add(num): which will wrap around
    - checked_add(num): it returns a Result enum
    - overflowing_add(num): returns value and overflow tuple (result, bool overflow)
    - saturating_add(num): it'll clamp to 255 if it goes above

```rs
fn main() {
    let x: u8 = 255;
    let y = x.wrapping_add(1); // Wraps around (255 -> 0)
    println!("Wrapping Add: {}", y);
    let x: u8 = 255;
    let y = x.checked_add(1);

    match y {
        Some(value) => println!("Checked Add: {}", value),
        None => println!("Overflow detected!"),
    }
    let x: u8 = 255;
    let (result, overflowed) = x.overflowing_add(1);
    println!("Result: {}, Overflowed: {}", result, overflowed); //would print 0, true

    let x: u8 = 255;
    let y = x.saturating_add(1); //will stick to 255.
    println!("Saturating Add: {}", y);
}

```

## Floating Point Types

| data type | precision |
| --- | --- |
| f32 | 32 bit float |
| f64 | 64 bit float |

## Numeric operations

```rs
fn main() {
    // addition
    let sum = 5 + 10;

    // subtraction
    let difference = 95.5 - 4.3;

    // multiplication
    let product = 4 * 30;

    // division
    let quotient = 56.7 / 32.2;
    let truncated = -5 / 3; // Results in -1

    // remainder
    let remainder = 43 % 5;
}
```

## Boolean types

```rs
fn main() {
    let t = true;

    let f: bool = false; // with explicit type annotation
}
```

## Character Types

```rs
fn main() {
    let c = 'z';
    let z: char = 'ℤ'; // with explicit type annotation
    let heart_eyed_cat = '😻';
}
```

## Compound Types

- Compound types can group multiple values into one type. There are two types:
    - tuples
    - arrays

## Tuple Types

```rs
fn main() {
    let tup: (i32, f64, u8) = (500, 6.4, 1);
    let (x,y,z) = tup;
    println!("the value of y is {}",y); //we can use tup.1 as well.
}
```
- A tuple is a group of values with different types.


## Array types

- Arrays are a collection of multiple values of the same type:

```rs
fn main() {
    let a = [1, 2, 3, 4, 5];
    let a: [i32; 5] = [1,2,3,4,5];
    let a = [3; 5]; //creates array of size 5, each element is 3

    println!("{}",a[2]); //how to access an element
}
```

## Functions

- We can use th fn keyword for functions

```rs
fn main() {
    println!("Hello, world!");

    another_function();
    cool_coin(25);
}

fn another_function() {
    println!("Another function.");
}

fn cool_coin(x: i32){
    println!("{}",x);
}
```

## Statements and Expressions

- States are instructions to perform. It doesn't return a value.
- expressions evaluate to a resultant value.

```rs
fn main(){
    let y = 6; // a statement
    let y = {
        let x = 3;
        x + 1 
    } //this is a expression, because it returns x + 1, or 4.
}
```

## Functions with Return Values

```rs
fn five() -> i32 { //the -> specifies the return type.
    5
}

fn main() {
    let x = five();

    println!("The value of x is: {x}");
}
```

## If statements

```rs
fn main() {
    let number = 3;

    if number < 5 {
        println!("condition was true");
    } else {
        println!("condition was false");
    }
}
```
- Works similar to other programming languages, except you don't have parenthesis.

## if let

```rs
fn main() {
    let condition = true;
    let number = if condition { 5 } else { 6 };

    println!("The value of number is: {number}");
}
```

- Here, we assign the variable depending on the result of the if statement.

## Loops

```rs
fn main() {
    loop { //infinite loop
        println!("again!");
    } //you can have a return value

    let mut counter = 0;

    let result = loop {
        counter += 1;

        if counter == 10 {
            break counter * 2;
        }
    };

    println!("The result is {result}");

    while counter != 0 {
        counter = counter - 1;
    }

    let a = [10,20,30,40,50];
    for element in a { println!("value is {}",element);}

    for number in (1..4) {
        println!("{number}!");
    }
    println!("LIFTOFF!!!");
}
```

- The for loop will do 1, 2, 3. It won't do 4. If you want 4, you do ```(1..=4)```.