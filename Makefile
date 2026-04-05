CC = g++
CFLAGS = -Wall
TARGET = huffman

all: $(TARGET)
	g++ huffman.cpp -o huffman
debug:
	g++ -g huffman.cpp -o huffman

$(TARGET): huffman.cpp
	$(CC) $(CFLAGS) -o $(TARGET) huffman.cpp

clean:
		rm -f $(TARGET)
