![PickleLisp CI](https://github.com/Rad-hi/PickleLisp/actions/workflows/tests.yml/badge.svg)

## Building a custom LISP called PickleLisp

It started by following this book: https://www.buildyourownlisp.com/contents
Though from the start, I followed a slightly different coding style, and kept adding features of my own here and there as I was progressing through the code. Additionally, I added unit tests early on to help me move faster through the book, and to ensure a more certain iterating process.

### Brief

TODO

### Hello, World

```bash
cd PickleLisp/src
make clean && make

cd ..
./src/PickleLisp examples/hello_world.pickle
```

### Tests

I wrote tests myself without using any framework, so they're weirdly implemented, and are kinda hard to modify. But, they do the job.

```bash
cd PickleLisp/test
make clean && make

./test
```

### Syntax

TODO
