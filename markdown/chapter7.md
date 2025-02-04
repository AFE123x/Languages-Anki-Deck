# Chapter 7

## What is a module?

- Modules allow us to: 
    - organize code for readability and reuse
    - control scope and privacy
    - Contains traits, enums, functions, etc.

## How do you define a module

- You use the ```mod``` keyword for this.

## How can you view the heirarchy of the modules crate?

You can, after installing cargo-modules, can call the following function:

```bash
cargo-modules structure
```

- This will show you the modules, and the functions associated with it.

## pub keyword

```rs
mod front_of_house {
    pub mod hosting {
        fn add_to_waitlist() {}
    }
}

pub fn eat_at_restaurant() {
    // Absolute path
    crate::front_of_house::hosting::add_to_waitlist();

    // Relative path
    front_of_house::hosting::add_to_waitlist();
}
```

- If we were to try and compile this, it wouldn't work, because eat_at_restaurant wouldn't be able to access ```add_to_waitlist()``` since it's private.


## super keyword

- In modules, let's say we want to access the parent module. We'd use the ```super``` keyword, similar to ```..``` when exploring files.

## use keyword

- we can, instead of writing the path, use the use keyword to shorten the link.

```rs
mod front_of_house {
    pub mod hosting {
        pub fn add_to_waitlist() {}
    }
}

use crate::front_of_house::hosting::add_to_waitlist;

pub fn eat_at_restaurant() {
    add_to_waitlist();
}
```

## use as keyword

- We can rename paths using the as keyword:

```rs
use std::fmt::Result;
use std::io::Result as IoResult;

fn function1() -> Result {
    // --snip--
}

fn function2() -> IoResult<()> {
    // --snip--
}
```

## Putting files separately

- You can have multiple .rs files in the path path as main.rs. If you want to separate them in folders, you need to name the .rs file mod.rs
