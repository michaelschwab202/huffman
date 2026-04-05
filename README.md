#Huffman Encoding CLI
This repository contains source code for a command-line interface tool for encoding and decoding files with [Huffman coding](https://en.wikipedia.org/wiki/Huffman_coding).

##Installation
First, clone the repository onto your local machine.

```bash
git clone https://github.com/michaelschwab202/huffman.git
```

Then, compile the program. If you have Make, the easiest way is with that.

```bash
make
```
Otherwise, you can use your favorite C++ compiler to compile the program. Like with g++.

```bash
g++ huffman.cpp -o huffman
```
Either way, you should have the compiled executable in your directory.

##Usage
Run the program with an input file as an argument. You'll either be encoding or decoding this input file depending on whether you set the decoding flag (-d).
The program will encode/decode the input file and will write the result to the output file (which you can name with the -o option).
```bash
Usage: huffman input_file [-o output_file]
Options:
    -d    Decode the file (default is encode)
    -o    Give a name for the output file
```
