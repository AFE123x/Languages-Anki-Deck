# Chapter 11

## What are tests

- Tests are rust functions that verify that non-test code is functioning in the expected manner.
- Test functions perform three actions:
    - Set up any needed data or state
    - Run the code you want to test
    - Assert that the results are what you expect

```rs
pub fn add(left: usize, right: usize) -> usize {
    left + right
}

#[cfg(test)]
mod tests {
    use super::*; //we need to bring add into scope

    #[test]
    fn it_works() {
        let result = add(2, 2);
        assert_eq!(result, 4);
    }
}
```
- we use relative paths here
- If the main thread panics, the test fails.


## assert! macro

- The ```assert!()``` macro will poass if the argument is true


## assert_eq! macro

- The ```assert_eq!(a,b)``` macro will pass if a == b.

## assert_ne! macro

- the ```assert_eq!(a,b)``` macro will pass if a != b.

## ading custom error messages

- we can add an additional message to the assert macros to add a custom messages:

```rs
assert!(
    result.contains("Carol"),
    "Greeting did not contain name, actual name was {}",
    result
);
```

- This will print the Greeting message if the test failed

## How can you test that a function panics?

```rs
mod tests {
    use super::*;
    #[test]
    #[should_panic]
    fn greater_than_100() {
        Guess::new(200);
    }
}
```

- The ```#[should_panic]``` tag tellls us that ```Guess::new(200);``` should panic.


```rs
#[should_panic(expected = "this is the expected error message")]
```

- We can have multiple panic massages. We can test that we get the correct error message


## How to run your tests

```bash
cargo test
```

## How to test function that tests the result enum (using ? for example)

```rs
mod tests{
    #[test]
    fn it_works() -> Result<(),String> {
        if 2 + 3 == 4{
            Ok(())
        }
        else {
            Err(String::from("two plus three doesn't equal four!")
        }
    }
}
```

## Controlling how tests are run

```rs
#[cfg(test)]

mod tests{
    #[test]
    fn it_works1() -> Result<(), String> {
        if 2 + 2 == 4{
            Ok(())
        }
        else{
            Err(String::from("Two plus two is not equal to four"))
        }
    }
    #[test]
    fn it_works2() {
        assert_eq!(2 + 2, 4);   
    }
}
```

- We can control how the tests are run, we have additional options

- By default:
    - tests run on multiple threads.
        - To specify the number of threads, you can use ``` cargo test --test-thread n```, where it will run the tests on n threads.
    - print statements aren't shown.
        - To show output, you can do ```cargo test -- --show-output```
- To see other features available, you can use ```cargo test -- --help```

## Running particular tests

let's say that we want to run the test, it_works1 only. To run that, you can do:

```bash
cargo test it_works1
```

- Let's say there's also it_works2, you can write ```cargo test it_works```, it will test all programs that start with it_works.

- You can also run test based on the module they're in.

## [ignore] tag

- If you have tests that take a lot of time, you can use the ```#[ignore]``` tag on top of a test that takes time, so it doesn't run by default. 
- If you do want to run it, you'd explicitly write: ```cargo test -- --ignore```.


## `[cfg(test)]` tag

- This tag says that this module will only compile if ```cargo test``` is called

## tests folder

- If you made a library crate, you will have a tests folder. This is where all you r integration tests are located.
    - all your files are going to implicitly going to be unit tests

## Running a specific Integration test

you can run the following:

```bash
cargo test --test integration_test
```

- This will run all the tests in the test/integration_test
