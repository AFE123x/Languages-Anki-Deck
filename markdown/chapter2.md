## What are the five essential components to learning a language?

- Syntax
- Semantics
- Idioms
- Libraries
- Tools

## What is Syntax?

- Syntax are the rules that constitutes a well-formed program in the language.
    - THis includes things like keywords, restrictions on white space, operators, etc.


## What are semantics?

- Semantics are the rules that define the basic behaviors of a program.
- AKA, what does the program mean, what does the syntax represent?

## What are the two pieces to semantics?

- Dynamic semantics: run time behavior of a program as it's executed.
- Static semantics: compile time checks that ensures the program is legal.

## What are Idioms

- Idioms define the common approaches to using language features to express computation.

## Libraries

- Bundles of code that were written for you and will make you more productive.

## Tools

- Language implementations that provide a compiler or interpreter.
- There are other Tools, like debuggers, IDEs, etc.

## Types and Variables

- We can define variables as such:

```ocaml
let x = 42;;
let x: int = 42;; (* explicitly defines type*)
```

## Functions

```ocaml
let increment x = x + 1;;

let y = increment 10;;
print_int y;;
print_newline();;
```

- Functions follow the following convention:

```ocaml
val increment: int -> int = <fun>
```
- increment is the identifier
- int -> int is the return value and input
- fun is the function itself.

## recursive functions

```ocaml
let rec fact n =
    if n = 0 then
        1
    else
        n * fact (n-1)
```
- You need to specify `rec` when working with recursive stuff.

## Function Types

- OCaml convention is ```t1 -> t```
    - In this example, a function takes in type t1, and returns type t

## Modular code

- If you want, you can specify the function in another file

```ocaml
(* mycode.ml *)
let inc x = x + 1
```

```ocaml
(* main.ml *)
# #use "mycode.ml" (* This loads your code at the top level. *)

let x = inc 3;;
print_int x;;
print_newline();;
```

## how to compile ocaml code?

```bash
$ ocamlc -o hello hello.ml
$ ./hello
```

## Dune

- Dune is a build system which will compile and link code for us.

## Dune - how to compile and run

```bash
dune build #compiles your code
dune run #compiles and runs code
dune clean #cleans your code
```

## Dune - initialize project

```bash
dune init proj project_name
```

## running dune continuously

```bash
dune build --watch
```

## Assertions

- Assertions are functions that takes a boolean expression. 
    - if it's true, we good
    - If it's false, the program terminates.

```ocaml
let () = assert (f input1 = output1)
let () = assert (f input2 = output2)
let () = assert (f input3 = output3)
```

## if expression

- Conditional branching:

```ocaml
let x = if 3 + 5 > 2 then "yay!" else "boo!";;
print_endline x;;

```
- `3 + 5 > 2` is true, so it prints "yay!"

- We also have else-if

```ocaml
let x = if 3 + 5 < 2 then "yay!" 
else if 3 + 5 > 2 then "naur!"
else "boo!";;
print_endline x;;
```

## let expression

```ocaml
let result = 
  let x = 10 in  (* x is only accessible in this expression *)
  x + 5          (* The expression evaluates to 15 *)
in 
print_int result;; (* prints 15 *)
print_newline ();;
```

- This sets result equal to x where result = x + 5

## Constructing Lists

- `[]` depicts an empty list
- `e1 :: e2` prepends an element e1 to list e2
- `[e1 ; e2 ; ... ; en]` is the same as `e1::e1::...::en::[]`

## Pattern matching

- to pull lists apart, we use the `match` construct.

```ocaml
match e wth
| p1 -> e1
| ...
| pn -> en
```
- p1..pn are patterns consisting of:
    - `[]`
    - `::`
    - constants
    - pattern variables
- e1..en are branch expressions, which pattern variables in the corresponding pattern are bound.

## pattern matching - wildcards

- We can use `_` as a wildcard.

```ocaml
let describe_list lst =
  match lst with
  | [] -> "Empty list"
  | [_] -> "Single element list"
  | _ :: _ -> "A list with multiple elements"
;;

print_endline (describe_list []);;         (* Output: Empty list *)
print_endline (describe_list [42]);;       (* Output: Single element list *)
print_endline (describe_list [1; 2; 3]);;  (* Output: A list with multiple elements *)

```

## Pattern Matching - Shorthand

```ocaml
let hd (h::_) = h;
```

- this matches and gives the output
- Only works for single conditions.

## Pattern matching - recursion

```ocaml
let rec sum lst =
  match lst with
  | [] -> 0
  | x :: xs -> x + sum xs

(* Example usage *)
let my_list = [1; 2; 3; 4]
let total = sum my_list (* This will return 10 *)
```

## Scope