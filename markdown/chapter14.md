# Chapter 14

## Release and dev profiles

- When you compile and run a rust program, you can choose to create a dev version, or a final release version.

```bash
cargo build # builds dev profile
cargo build --release # builds release profile
```

- Cargo uses default settings for each profile, but you can explicitly setup things in the ```Cargo.toml``` file.

## Configuring release and dev profiles

```toml
[profile.dev]
opt-level = 0

[profile.release]
opt-level = 3

```

- We can use ```[profile._]``` to set the profile of the dev or release profile.

## Cargo compiler optimization levels

- Similar to gcc and g++'s O1, O2, O3 optimizations, rust also has optimizations available, ranging from 0 - 3

- You can set the optimization level in the ```Cargo.toml``` file.

```toml
[profile.dev]
opt-level = 3
```

## Making documentation comments

- You use the triple slash ```///``` to make documentation comments, which will appear as html documentation.
- YOU CAN USE MARKDOWN!!!

- You can try it out and open the documentation using ```cargo doc --open```


## Rust markdown - sections:

- Examples: Example use
- Panics: Scenarios where the program can panic.
- Errors: What does the function return, what kind of errors can occur
- Safety: If the function is unsafe, explain why it's unsafe.

## Rust markdown, Crate level documentation

- You can make documentation for the entire crate using ```//!```.

## metadata to crates

- In the cargo.toml, youc an specify the name, license, description, edition, license and much more.

```toml
[package]
name = "guessing_game"
version = "0.1.0"
edition = "2025"
description = "a fun game where you guess what the computer chose"
license = "MIT OR Apache-2.0"
```

## Publishing to crate.io

```bash
$ cargo publish
```

You just need to write cargo publish, and it'll publish it to crates.io

## What is the semantic versioning convention?

- given a version number X.Y.Z:
    - X is major versions that make incompatible API changes
    - Y: Minor version is when you add functionality in a backwards compatible manner
    - Z: Patches for things like small bugs.

## How do you deprecate a specific crate version? How can you undo it?

- You use yank to deprecate a specific version specified

```bash
cargo yank --vers 1.0.1
```

- To undo it, you can do ```cargo yank --vers 1.0.1 --undo```

## What is a workspace?

- A workspace is a set of packages that share the same Cargo.lock and output directory.

## How do you create a workspace for a binary crate?

- You make the directory named after whatever you want to name the workspace.

```bash
$ mkdir add
$ cd add
```
- you'd then create a Cargo.toml, then enter the following:
```toml
[workspace]

members = [
    "adder",
]
```

- You can then use cargo to make a crate (you can make libraries, binaries, etc.):

```bash
cargo new adder
```

- You can add dependencies to certain crates under the dependencies section:
```toml
[dependencies]
add_one = { path = "../add_one" }
```


## How do you build, run and test a certain workspace?

- You specify the ```-p``` flag, and you specify the workspace name:

```bash
$ cargo test -p add_one
```


## Installing Binaries with cargo install

- You can install binary crates using the cargo install command.
- It'll be stored in the root bin folder, or in ```~/.cargo/bin```
