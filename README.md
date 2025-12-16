# femto
Compile-time template-expression-based library for solving Schroedinger equations with automatic differentiation.

## General Information
This software has been develope using:
- Ubuntu 24
- gcc-13
- C++20

## Functionality
This library uses template expressions to represent mathematical formulae. The building blocks consist of `Constants` and `Variables` which can be used to create `Functions`, for example:

```c++
Constant pi(3.14);
Variable<R> r;
Function l = Constant(2) * pi * r;
std::cout << l(R{2.}) << "\n"; // returns 12.56
```
GIves us a function represneting a circumference of a cricle. Variables in femto are represented by functors (my goal is to have them represented as strong types, for now it's functors) with an `operator()()` which just returns the stored value:

```c++
struct R
{
    double r;
    constexpr double operator()(){return r;}
};
```

Each arithmetic operation is represented with a type which means that our resulting `Function` becomes an object of nested operations, variables, and constants, e.g. `Function l` from our last example is of type `Function<BinaryOp<BinaryOp<Constant<int>, Constant<double>, BinaryOperations::Multiplication>, Variable<R>, BinaryOperations::Multiplication>>`. Yes, this is a mouthful, but because everyting here is known at compile-time it allows the compiler to optimise the actual calculation of the circumference.

The functions can have an arbitrary number of variables, e.g.:

```c++
Variable<X> x;
Variable<Y> y;
Function f = x * y + x / y;
std::cout << f(X{1.},Y{2.}) << "\n"; // returns 2.5
```

femto knows how many variables it expects and will fail to compile if a wrong number of arguments is provided or if passed types don't match the expected ones. Something I've failed to find in many examples on the internet is the multidimensionality of the template expressions. So I've done it myself, because I need it for the future.

I've implemented a few mathematical functions (all are constexpr):
- Square root using Newton - Rapson method
- Exponent using the Taylor series expansion
- Factorial
- Power

I have also implmeneted automatic differentiation:

```c++
Variable<X> x;
Variable<Y> y;
Function f = x * y + x / y;
Function dfdx = D(f,DiffWrt(x)); // is equaivalent to y + 1 / y
```

The differentiation requires a variable to be specified. My implementation is closer to a partial derivative $\frac{df}{dx}$ rather than a full gradient calculation $\nabla f$. It can be nested to yield higher order derivatives, e.g. following the last example:

```c++
Function d2fdx2 = D(D(f,DiffWrt(x)),DiffWrt(x)); // equivalent to 0
Function d2fdxdy = D(D(f,DiffWrt(x)),DiffWrt(y)); // equivalent to 1 - 1 / (y ^ 2)
```

## Current Status
Currently the library is in highly developer stage. It is possible to evaluate functions and to differetiate them. There exists a first attempt in the implementation of a wavefunction, but it will surely be scraped. There are also three executables which I use for testing out some things.

## Future Plans
1. Implement a root finder and numeric solver which utilise the automatic differentiation
2. Implement calculation on a grid for calculations which are too comlex to be done analytically
3. Implement N-dimensional numeric integration
4. Implement a way to simplify the expressions, e.g. $x * y + x - x + y = y * (x + 1)$