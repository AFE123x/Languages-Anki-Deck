# Chapter 6

## How do you define and initialize an enum?

You can define an enum by using the enum keyword:

```rs
enum IpAddrKind {
    V4,
    V6,
}

fn main(){
    let four = IpAddrKind::V4;
    let six = IpAddrKind::V6;
}
```

## What differentiates enums in Rust compared to languages like C, C++, etc.

- We can store values in enums

```rs
enum IpAddr {
    V4(String),
    V6(String),
    AV4(i32,i32,i32),//another way to represent enums.
    AV4{f: i32, g: i32, h: i32}, //another representation
}

fn main(){
    let home = IpAddr::V4(String::from("127.0.0.1"));
    let loopback = IpAddr::V6(String::from("::1"));
}
```

## Options enum

- Instead of having the NULL pointer, rust utilizes the **Options** enum:

```rs
enum Options<T> {
    None,
    Some(T),
}
```

## What is wrong with the following code?

```rs
    let x: i8 = 5;
    let y: Options<i8> = Some(5);

    let sum = x + y;
```

We're trying to add a options enum to an i8 type. We need to use the ```unwrap()``` method to get 5.

## Match Control Flow

- It's similar to the switch case statement in other languages:

```rs
enum Coin {
    Penny,
    Nickel,
    Dime,
    Quarter(String),
}

fn value_in_cents(coin: Coin) -> u8 {
    match coin {
        Coin::Penny => {
            println!("You got da penny!!!");
            1
        },
        Coin::Nickel => 5,
        Coin::Dime => 10,
        Coin::Quarter(state) => {
            println!("Quarter from {}",state);
            25
        },
        _ => 69,
    }
}
```

It'll match coin, with whatever enum you have. If you have a value in the Quarter enum, you can include that in the match statement.
