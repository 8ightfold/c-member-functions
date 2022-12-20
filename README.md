## Details
This is a single header utility that lets you register and call normal C functions like C++ member functions. The only dependency is the [Boost Preprocessor](https://github.com/boostorg/preprocessor) library.

## Registering
To enable "reflection" on functions, you must supply the base type and necessary functions to the ``$register`` macro. Functions must be passed with the syntax ``(return_type)(name)(arguments...)``. Here is an example:
```c
$register(Foo, void(bar)(Foo*));
```
To define a member as usable, you must use the ``$def`` macro. You can then use the ``$`` macro to access the table. An example is:
```c
$def(Foo, foo) = { 5 };
$(foo)->bar();
```

## Notes
Pushed this out as soon as I finished so there are limitations.
First, you cannot use pointer types as it will default to taking the address. Second, complex types can only be used if typedefed (eg. function pointer types). You should also be careful with the types of functions you use, as the C -> C++ type conversion is hardcoded.
