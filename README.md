# first-step

Take your first step in writing a compiler.

## Building from Source

Before building `first-step`, please make sure you have installed the following dependencies:

* `cmake` 3.13 or later
* C++ compiler supporting C++17

You can build this repository by executing the following command lines:

```
$ git clone --recursive https://github.com/pku-minic/first-step.git
$ cd first-step
$ mkdir build
$ cd build
$ cmake .. && make -j8
```

## Getting Started

There are some example `first-step` source programs in the `examples` directory, such as `fib.fstep`:

```first-step
# calculate the nth term of the Fibonacci sequence
fib(n) {
  if n <= 2 {
    return 1
  }
  else {
    return fib(n - 1) + fib(n - 2)
  }
}

main() {
  print(fib(input()))
  return 0
}
```

You can evaluate this program using the interpreter by running:

```
$ build/fstep examples/fib.fstep
20
6765
```

Or compile it to RISC-V assembly:

```
$ build/fstep examples/fib.fstep -c -o out.S
$ cat out.S | less
```

## EBNF of first-step

```ebnf
Program       ::= {FunctionDef};
FunctionDef   ::= IDENT "(" [ArgsDef] ")" Block;
ArgsDef       ::= IDENT {"," IDENT};

Block         ::= "{" {Statement} "}";
Statement     ::= IDENT ":=" Expression
                | IDENT "=" Expression
                | FunctionCall
                | IfElse
                | "return" Expression;
IfElse        ::= "if" Expression Block ["else" (IfElse | Block)];

Expression    ::= LOrExpr;
LOrExpr       ::= LAndExpr {"||" LAndExpr};
LAndExpr      ::= EqExpr {"&&" EqExpr};
EqExpr        ::= RelExpr {("==" | "!=") RelExpr};
RelExpr       ::= AddExpr {("<" | "<=") AddExpr};
AddExpr       ::= MulExpr {("+" | "-") MulExpr};
MulExpr       ::= UnaryExpr {("*" | "/" | "%") UnaryExpr};
UnaryExpr     ::= ["-" | "!"] Value;
Value         ::= INTEGER
                | IDENT
                | FunctionCall
                | "(" Expression ")";
FunctionCall  ::= IDENT "(" [Args] ")";
Args          ::= Expression {"," Expression};
```

## License

Copyright (C) 2010-2021 MaxXing. License GPLv3.
