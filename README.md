![PickleLisp CI](https://github.com/Rad-hi/PickleLisp/actions/workflows/tests.yml/badge.svg)

## Building a custom LISP called PickleLisp

It started by following this book: https://www.buildyourownlisp.com/contents.

Though, from the start, I followed a slightly different coding style, and kept adding features of my own here and there as I progressed through the book. Additionally, I added unit tests early on to help me move faster, and to ensure a more certain iterating process.

### Disclaimers

- This language is not useful, and is here only for educational purposes.

- Prior to following this book, I knew nothing about LISPs, and I kinda still dunno, so don't expect this language to be a "proper" LISP.

### Dependency

For the REPL to have history, we use this library.

```bash
sudo apt install libedit-dev
```

### Hello, World

```bash
cd PickleLisp
make clean && make  
# or you can do `make -B`

./pickle examples/hello_world.pkl
```

### "install" PickleLisp on your system (accessibility within any terminal)

If you want to just do ```pickle example.pkl``` from anywhere you go, I suggest adding an alias to your `.bashrc` file:

```bash
nano ~/.bashrc
# go to the bottom of the file, and add:
alias pickle='/PATH/TO/PICKLELISP/PARENT/DIR/PickleLisp/pickle'

# if it's under home, you can do: ~/PickleLisp/pickle
```
> don't forget to source the `.bashrc` in current terminal

You might want to put PickleLisp in `/opt` or somewhere else where it wouldn't bother you.

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

- [RayLib: https://github.com/raysan5/raylib](https://github.com/raysan5/raylib/tree/master)

- [libffi: https://github.com/libffi/libffi](https://github.com/libffi/libffi)

- [libffi Docs: https://www.chiark.greenend.org.uk/doc/libffi-dev](https://www.chiark.greenend.org.uk/doc/libffi-dev/html/The-Basics.html)
