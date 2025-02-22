## How would you read standard input from the user?

```rs
use std::io; //needed to io::stdin dependency

fn main(){
    println!("enter something");
    let mut guess = String::new(); //creates a string type

    io::stdin().read_line(&mut guess).expect("Failed to read line");
    printnln!("You guessed {}",guess);
}
```
- ```String::new()``` creates a new string type
- ```io::stdin()``` actually reads you output:
    - ```read_line(&mut guess)``` will read the input and store it in guess
    - ```expect```, if read_line fails, will print out the message specified.
        - If you don't use this, the compiler may warn you about it.

## storing values with variables

- unlike other languages, we don't need to specify the type in rust, the compiler can imply it:

```rs
let guess = 5; //will declare variable 5
let mut franny = 20; //will declare variable with 20
```
- immutable types cannot be modified
- mutable types can be modified.

## What does the ``::`` mean?

```rs
let mut guess = String::new();
```

- the ``::`` tells us that ``new()`` belongs to the new instance of String.

## What is a crate?

- A crate is a collection of Rust source code files.
- A Binary crate is an executable.

## How do we generate random numbers? What crate do we use?

- We use the ``rand`` crate.
- You can include it in the dependencies section of your toml file:

```toml
[dependencies]
rand = "0.8.5"
```

- Version numbers vary.

## What is the cargo.lock file?

- The cargo lock file will ensure that, unless you explicitly change the dependency version, if someone else rebuilds it, it won't use a newer version, which may potentially break your program.

## How to update crates

you can use ```cargo update``` to update your crates.

## How do you generate a random number between 1 and 100?

```rs
let secret_number = rand::random_range(0..=100);
```

- this will generate a secret_number such that $1 \leq \text{secret_number} \leq 100$.

## How can you view documentation based on your dependencies?

```rs
cargo doc --open
```

- This will open a page containing all the documentation you need for the dependencies in your program.

## How could you convert a string into an integer?

```rs
let mut guess = String::new();
io::stdin().read_line(&mut guess).expect("FAILED!");
let guess: u32 = guess.trim().parse().expect("Enter a valid number");
```

- ```.parse()``` will convert your string to an int.
- ```.expect()``` is for error handling.

## Make a rust program that generates a random number (make it run until you guess the number correctly)

```rs
use rand;
use std::io;
fn main() {
    loop {
        let y = rand::random_range(0..=100);
        let mut guess = String::new();
        io::stdin().read_line(&mut guess).expect("input reading FAILED!");
        let result: u32 = guess.trim().parse().expect("Unable to parse");
        if result == y{
            println!("You guess correctly");
            break;
        }
        else if result < y{
            println!("{} < {}",result,y);
        }
        else{
        println!("{} > {}",result,y);
        }
    }
}
```