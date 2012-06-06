// HuffmanC.cpp : Defines the entry point for the console application.
//

#include "FileBuffer.h"
#include "Huffman.h"

#include <iostream>

using namespace std;

#include <stdio.h>

int main(int argc, char* argv[])
{
	{
		FileReadBuffer reader("d:\\a.txt");

		FileWriteBuffer writer("d:\\a.hfmz");
		HuffmanEncoder compresor;
		compresor.compress(reader, writer);
		cout << "header: " << sizeof(HuffmanFileHeader) <<  " data:  " << compresor.compressSize << " size: " << compresor.srcSize ;
	}

	{
		FileReadBuffer reader("d:\\a.hfmz");
		FileWriteBuffer writer("d:\\b.txt");
		HuffmanDecoder decompresor;
		decompresor.decompress(reader, writer);
	}

	//FILE* fp = fopen("d:\\ccc.txt", "w+");
	//char d = 'c';
	//for (size_t i = 0; i< 40000; ++i)
	//	fwrite(&d, 1, 1, fp);
	//fseek(fp, 0, SEEK_SET);
	//d = 'A';
	//fwrite(&d, 1, 1, fp);
	//fclose(fp);
	return 0;
}

