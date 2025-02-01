## How do you write Hello World? How do you compile said code?

```rust
fn main(){
    println!("Hello, world!");
}
```

```bash
$ rustc main.rs
$ ./main
Hello, world!
$ 
```

## What is Cargo?

Cargo is Rust's build system and package manager. It handles most tasks, like build code, downloading libraries and building libraries.

## How do you build projects with cargo?

```bash
$ cargo new hello_cargo
$ cd hello_cargo
```

- If you don't want to use version control, you can do ```--vcs=none```.

## What is a toml file

- The toml file (Tom's Obvious, Minimal Language), is Cargo's configuration format.

## How do you compile and run Rust programs using cargo? How can you check for errors

```bash
$ cargo build # compiles your rust code
$ cargo run # runs your rust code
$ cargo check # checks your code with errors.
```

## How do you build for release using cargo?

You would run cargo build ```--release``` to compile it with optimizations. This will not compile your code with targot or debug metadata.