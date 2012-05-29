// HuffmanC.cpp : Defines the entry point for the console application.
//

#include "FileBuffer.h"
#include "Huffman.h"

int main(int argc, char* argv[])
{
	FileReadBuffer reader("d:\\lua-5.2.0.tar");
	FileWriteBuffer writer("d:\\abc.hfmz");
	HuffmanEncoder compresor;
	compresor.compress(reader, writer);

	return 0;
}

