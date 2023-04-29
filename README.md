# p2c
A simple PNG to C compiler (P2C)

# Usage:
```console
$ ./p2c -i <input file> -o <output file>
```

```
$ ./p2c -h
PNG 2 C Compiler
version: 0.1-dev

Usage:
        -i              The input file used to convert
        -o              The output file to write after converting
        -v              Show the program version
        -h              Show this help menu

```

# Examples
> See [examples/print_image.c](https://github.com/D4yvid/p2c/tree/main/examples/print_image.c)

# Building
```
$ git submodule update --init --recursive       # Clone the STB repository
$ make                                          # Build the p2c binary
```

# Testing
```
$ make test
```
