# chapter 8

## What is a Common Collection?

- A common collection is a data structure that contains multiple values.
- Unlike the array and tuple type, collections are stored on the heap. 
    - This means that it can grow and shrink during runtime.

## What are the three collections types rust offers?

- Vector: store variables adjacent to each other.
- String: heap allocated string
- hashmap: Let's you associate a value with a key.

## How can you create a vector?

```rs
let v: Vec<i32> = Vec::new(); //creates an empty vector
let v = vec![1,2,3]; //macro which initialized vector with 1, 2 and 3
```

## How do you add elements to a vector?

```rs
let mut v = Vec::new();

v.push(5);
v.push(6);
v.push(7);
```

## How to reference a vector element, or get element:

```rs
    let v = vec![1, 2, 3, 4, 5];

    let third: &i32 = &v[2];
    println!("The third element is {third}");

    let third: Option<&i32> = v.get(2);
    match third {
        Some(third) => println!("The third element is {third}"),
        None => println!("There is no third element."),
    }

```

- You can use ```&v[2]``` to get the second element.
    - It's prone to errors.
- ```v.get(2)``` also gets the second element, but returns the option enum:
    - AKA, You can handle if it's out of bounds.


## Is there any issues with this code

```rs
    let mut v = vec![1, 2, 3, 4, 5];

    let first = &v[0];

    v.push(6);

    println!("The first element is: {first}");

```

- Yes, you're making a mutable reference after doing a immutable borrow previously. The problem is that you're using the immutable reference after doing a mutable action.

## Iterating over values in a vector

```rs
let v = vec![100, 32, 57];
for i in &v {
    println!("{i}");
}

```
- This will iterate through all values in vector v.

```rs
let mut v = vec![100, 32, 57];
for i in &mut v {
    *i += 50;
}

```
- This will multiply all values in the vector my 50.

## String type

- The string type, unlike string literals, are stored in the heap:

### Creating a new string:

```rs
    let mut s = String::new();
    let data = "initial contents";

    let s = data.to_string();

    // the method also works on a literal directly:
    let s = "initial contents".to_string();
    let s = String::from("Initial content");
```


## Appending to a string

You can append to strings using the ```push_str``` method.

```rs
    let mut s = String::from("foo");
    s.push_str("bar");

    let mut s1 = String::from("foo");
    let s2 = "bar";
    s1.push_str(s2);
    println!("s2 is {s2}");

```
- This appends a string.

```rs
    let mut s = String::from("lo");
    s.push('l');
```
- To append a single character, you can use push().

## Concatenation with the + Operator or the format! Macro

- You can use concatenation, similar to java

```rs
    let s1 = String::from("Hello, ");
    let s2 = String::from("world!");
    let s3 = s1 + &s2; // note s1 has been moved here and can no longer be used
```

- the format macro is pretty similar to sscanf in C:

```rs
    let s1 = String::from("Hello, ");
    let s2 = String::from("world!");
    let s3 = s1 + &s2; // note s1 has been moved here and can no longer be used
```

## Iterating over strings

```rs
for c in "Зд".chars() {
    println!("{c}");
}
```

- This will let you iterate through each character.

```rs
for b in "Зд".bytes() {
    println!("{b}");
}
```

- This will convert your string into bytes (the UTF representation).

## Creating a HashMap

```rs
    use std::collections::HashMap;

    let mut scores = HashMap::new();

    scores.insert(String::from("Blue"), 10);
    scores.insert(String::from("Yellow"), 50);

```

- You need to import ```std::collections::HashMap```.

## How to get a value from a HashMap

```rs
    use std::collections::HashMap;

    let mut scores = HashMap::new();

    scores.insert(String::from("Blue"), 10);
    scores.insert(String::from("Yellow"), 50);

    let team_name = String::from("Blue");
    let score = scores.get(&team_name).copied().unwrap_or(0);
```

- You use the ```.get()``` method to get the value. 
    - ```.get()``` returns an options enum
    - ```copied()``` will convert the Options<&T> to a Options<T>.
    - ```unwrap_or()``` unwraps the value, and gets T. If the enum is none, we give 0.

## How to iterate over hashmap:

```rs
    use std::collections::HashMap;

    let mut scores = HashMap::new();

    scores.insert(String::from("Blue"), 10);
    scores.insert(String::from("Yellow"), 50);

    for (key, value) in &scores {
        println!("{key}: {value}");
    }

```

- We use the for loop for this.

## HashMaps and Ownership

```rs
    use std::collections::HashMap;

    let field_name = String::from("Favorite color");
    let field_value = String::from("Blue");

    let mut map = HashMap::new();
    map.insert(field_name, field_value);
```

- After inserting the strings, the hashmap now owns the data. You do not own them, so you cannot use them after the insert.

## Adding a key and value only if key isn't present:

```rs
    use std::collections::HashMap;

    let mut scores = HashMap::new();
    scores.insert(String::from("Blue"), 10);

    scores.entry(String::from("Yellow")).or_insert(50);
    scores.entry(String::from("Blue")).or_insert(50);

    println!("{scores:?}");

```
- If Yellow doesn't exist, it'll insert 50.
- Blue does exist, so it'll stay as 10.
