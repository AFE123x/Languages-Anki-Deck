# Chapter 12

## how do you view command line arguments in rust?

```rs
use std::env;

fn main(){
    let args: Vec<String> = env::args().collect();
    dbg!(args);
}
```

to pass arguments into a rust program, you do:

```rs
cargo run -- args1 args2 args3 
```

- the ```--``` is used as a delimiter so, cargo doesn't mix up the arguments between your program and cargo.

## How do you read from a file?

```rs
use std::fs::File;
fn main(){
    let contents = fs::read_to_string(file_path)
    .expect("Failed to read file!");
    println!("{}",contents);
}
```

## Environment Variables

- Environment variables let us set the environment variables once, and have them set throughout the life of the terminal session.
- To use environment variables, we use ```std::env```.

```rs
use std::env;

fn main(){
    let ignore_case = env::var("IGNORE_CASE").is_ok();
}
```

- ```env::var()``` returns a Results enum
- ```.is_ok()``` checks if the value is a value, or an error

## How to set and unset environment variables

```bash
FRANNY=1 cargo run
```

- This will run the rust program with an environment variable, FRANNY

## Printing to Standard Error

- we can use ```eprintln!()``` to print to stderr

```rs
fn main() {
    let args: Vec<String> = env::args().collect();

    let config = Config::build(&args).unwrap_or_else(|err| {
        eprintln!("Problem parsing arguments: {err}");
        process::exit(1);
    });

    if let Err(e) = minigrep::run(config) {
        eprintln!("Application error: {e}");
        process::exit(1);
    }
}
```


