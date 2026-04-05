# Huffman Encoding CLI
This repository contains source code for a command-line interface tool for encoding and decoding files with [Huffman coding](https://en.wikipedia.org/wiki/Huffman_coding).
Generally, Huffman coding is useful for compressing/decompressing files.

## Installation
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

## Usage
Run the program with an input file as an argument. You'll either be encoding or decoding this input file depending on whether you set the decoding flag (-d).
The program will encode/decode the input file and will write the result to the output file (which you can specify with the -o option).
```bash
Usage: huffman input_file [-o output_file]
Options:
    -d    Decode the file (default is encode)
    -o    Give a name for the output file
```
### Example Usage
Let's say I have **hamlet.txt** and I'd like to encode it. I'd like to name the encoded file **hamlet_encoded.txt**. My huffman program is in my current directory.
```bash
./huffman hamlet.txt -o hamlet_encoded.txt
```
Now, I have my encoded text file. Now, I'd like to decode that file, and have that decoded output be called **hamlet_decoded.txt**. To do that, I would set the **-d** flag.
```bash
./huffman hamlet_encoded.txt -d -o hamlet_decoded.txt
```
Now, I see my decoded file in my current directory. If I wanted to choose another directory for my output, I can specify that in the output file's name.
```bash
./huffman hamlet_encoded.txt -d -o ./other_directory/hamlet_decoded.txt
```
## Adding the Program as a Command
If you wanted to be able to call the program globally on your machine, you can copy the executable to your PATH.
On Linux-based systems, you can do something like this:
```bash
sudo cp path/to/huffman/program/huffman /usr/local/bin
```
Then, you'll be able to call Huffman as a command.
