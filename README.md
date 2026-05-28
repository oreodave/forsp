# Forsp: A Forth+Lisp Hybrid Lambda Calculus Language

Forsp is a hybrid language combining Forth and Lisp.

Forsp is a minimalist language.

## Features

Forsp has:
  - An S-Expression syntax like Lisp
  - Function abstraction like Lisp
  - Function application like Forth
  - An environment structure like Lisp
  - Lexically-scoped closures like Lisp (Scheme)
  - Cons-cells / lists / atoms like Lisp
  - A value/operand stack like Forth
  - An ability to express the Lambda Calculus
  - A Call-By-Push-Value evaluation order
  - Only 3 syntax special forms: ' ^ $
  - Only 1 eval-time special form: quote
  - Only 10 primitive functions need to self-implement
  - Ability to self-implement in very little code

## Discussion

See blog post for details: [Forsp: A Forth+Lisp Hybrid Lambda Calculus
Language](https://xorvoid.com/forsp.html)

## Tutorial

The best way to learn Forsp is to [read the
tutorial](examples/tutorial.fp) along with the other available
[examples](examples/).

## Requirements
- C compiler that can compile C23 (tested: gcc 16.1.1 20260430 clang
  22.1.5)
- Make (test: GNU Make, bmake)

## Building and Demo

Building:
```
make
```

NOTE: Tested on:
- Mac M1
- x86-64 Linux with gcc 16.1.1

Running:
```
make run ARGS=<...>
```

Examples:
```
make examples
```
