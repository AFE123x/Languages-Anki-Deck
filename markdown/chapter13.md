# Chapter 13

## What are closures

- Closures are anonymous functions
    - They can be passed in as parameters
    - Can be assigned to local variables
- This is very similar to lambda notation is other languages
```rs
fn main() {
    let add = |x, y| {
        println!("calculating {} + {}",x,y);
        x + y
    };
    println!("Sum: {}", add(3, 5));
}
```

## Return type of closures

- We can explicitly specify the types of closures, similar to functions.
    - It's not necessary tho.
- This brings up a question, what would the return value be?

```rs
fn main(){
    let add = |x, y|{
        println!("calculating {} + {}",x,y);
        x + y
    };
    println!("sum: {}",add(3,5));
    // println!("sum {}",add(String::from("aaa"),String::from("bbb"));
}
```

- We look at the first call to add, and make it's return type the return type.
- The second print statement wouldn't work, because we implied the return type already.


## Iterators

- iterators allow us to iterate through data structures.
    - AKA, they handle the logic of iterating over each item, and when you reach the end. 

- In rust, iterators are lazy, meaning that they have no effect until you call methods that consume the iterator to use it up.

## How do you get an iterator from a data structure?

```rs
    let v1 = vec![1, 2, 3];

    let v1_iter = v1.iter();

    for val in v1_iter {
        println!("Got: {val}");
    }

```

- You use ```.iter()``` to get the iterator.

## Iterator Trait and Next Method

- The iterator is a trait, with a function, next:

```rs
pub trait Iterator {
    type Item;

    fn next(&mut self) -> Option<Self::Item>;

    // methods with default implementations elided
}

```

## Iterator methods

```rs
    #[test]
    fn iterator_sum() {
        let v1 = vec![1, 2, 3];

        let v1_iter = v1.iter();

        let total: i32 = v1_iter.sum();

        assert_eq!(total, 6);
    }
```

- Functions where, in the implementation, call next, are called consuming adapters.
- IN this example, the sum method will take ownership of the iterator, which will call ```nex()```, and adds the elements in the vector to it's return type.

## Producing iterators - map function

- We can create iterators from iterators:

```rs
    let v1: Vec<i32> = vec![1, 2, 3];

    let v2: Vec<_> = v1.iter().map(|x| x + 1).collect();

    assert_eq!(v2, vec![2, 3, 4]);

```
- Map will take in a closure, and instructs what to do with an item, x.

## Producing Iterators - filter

```rs
fn main() {
    let numbers = vec![1, 2, 3, 4, 5, 6, 7, 8, 9, 10];
    let evens: Vec<i32> = numbers.iter().filter(|&x| x % 2 == 0).copied().collect();
    
    println!("Even numbers: {:?}", evens);
}

```
- Here, we filter numbers by whether they're equal
