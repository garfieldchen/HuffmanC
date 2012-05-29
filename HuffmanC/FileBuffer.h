#ifndef _FILE_BUFFER_H
#define _FILE_BUFFER_H

#include <stdio.h>
#include <assert.h>

#include "Common.h"

struct BufferData {
	BufferData(byte* buff = 0 , size_t sz = 0)
		:buffer(buff), size(sz)
	{}

	byte* buffer;
	size_t size;
};

class IOReader {
public:
	virtual BufferData read(size_t size) = 0; 
	virtual Bit readBits(size_t bits) = 0;

	virtual void rewind() = 0;
};

class IOWriter {
public:
	virtual size_t write(const byte* data, size_t size) = 0;
	virtual size_t writeBit(const Bit& bit) = 0;
};

class FileReadBuffer
	:public IOReader
{
public:
	FileReadBuffer(const char* fn, size_t bufferSize = 1024* 1024 * 1024);
	~FileReadBuffer();

	BufferData read(size_t size);
	Bit readBits(size_t size);
		
	void rewind();
	size_t size();

	byte* buffer;
	byte* ptr;
	const size_t capacity;
	size_t realSize;
	FILE* file;
	size_t bitOffset;
};

class FileWriteBuffer
	:public IOWriter
{
public:
	FileWriteBuffer(const char* fn, size_t bufferSize = 1024* 1024 * 1024);
	~FileWriteBuffer();

	size_t write(const byte* data, size_t size);
	size_t writeBit(const Bit& bit);

	byte* buffer;
	byte* ptr;
	const size_t capacity;
	FILE* file;
	size_t byteOffset;
};

#endif