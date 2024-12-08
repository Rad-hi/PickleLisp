![PickleLisp CI](https://github.com/Rad-hi/PickleLisp/actions/workflows/tests.yml/badge.svg)

## Building a custom LISP called PickleLisp

It started by following this book: https://www.buildyourownlisp.com/contents.

Though, from the start, I followed a slightly different coding style, and kept adding features of my own here and there as I progressed through the book. Additionally, I added unit tests early on to help me move faster, and to ensure a more certain iterating process.

### Brief

I know nothing about LISPs, or functional programming. I found a book, and decided I'd get better at C, while learning a bit about functional programming through writing a LISP.

This language is not useful, and is here only for educational purposes.

### Hello, World

```bash
cd PickleLisp
make clean && make

./PickleLisp examples/hello_world.pickle
```

### Tests

I wrote tests myself without using any framework, so they're weirdly implemented, and are kinda hard to modify. But, they do the job.

```bash
cd PickleLisp
make clean && make

./test
```

### Syntax

TODO

### References

- [RayLib](https://github.com/raysan5/raylib/tree/master).

- [libffi](https://github.com/libffi/libffi)

- [libffi Docs](https://www.chiark.greenend.org.uk/doc/libffi-dev/html/The-Basics.html)

- [Article about system calls](https://blog.packagecloud.io/the-definitive-guide-to-linux-system-calls/)
