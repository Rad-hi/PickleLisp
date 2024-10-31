![PickleLisp CI](https://github.com/Rad-hi/PickleLisp/actions/workflows/tests.yml/badge.svg)

## Building a custom LISP called PickleLisp

Following the book: https://www.buildyourownlisp.com/contents

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
