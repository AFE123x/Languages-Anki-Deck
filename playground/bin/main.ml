let result = 
  let x = 10 in  (* x is only accessible in this expression *)
  x + 5          (* The expression evaluates to 15 *)
in 
print_int result;;
print_newline ();;
