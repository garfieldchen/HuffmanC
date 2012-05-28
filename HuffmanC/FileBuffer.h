#ifndef _FILE_BUFFER_H
#define _FILE_BUFFER_H

#include <stdio.h>
#include <assert.h>

typedef unsigned char byte;

struct BufferData {
	BufferData(byte* buff = 0 , size_t sz = 0)
		:buffer(buff), size(sz)
	{}

	byte* buffer;
	size_t size;
};

struct Bit{
	typedef long BitsType;

	Bit()
		:bits(0), len(0)
	{}

	Bit(const Bit& bit) {
		set(bit);
	}

	BitsType bits;
	int len;

	void addBit(int b) {
		assert(len < 64);
		if (b)
			bits |= 0x1 << len;

		++len; 
	}

	void set(const Bit& bit) {
		bits = bit.bits;
		len = bit.len;
	}

	void add(const Bit& bit) {

	}

	void add(BitsType bits, size_t size)
	{

	}
};

class IOReader {
	virtual Bit readBits(int bits) = 0;
};

class IOWriter {
	virtual size_t writeBytes(const byte* data, size_t size) = 0;
	virtual size_t writeBit(const Bit& bit) = 0;
};

class IOBuffer {
public:
	virtual BufferData read(size_t size) = 0;
	virtual size_t write(byte* data, size_t size) = 0;

	virtual Bit readBit(size_t size) = 0;
	virtual size_t writeBit(const Bit& bit) = 0;

	virtual void rewind() = 0;
};

class FileReadBuffer
	:public IOBuffer
{
public:
	FileReadBuffer(const char* fn, size_t bufferSize = 1024* 1024 * 1024);
	~FileReadBuffer();

	BufferData read(size_t size);
	Bit readBit(size_t size);
	
	size_t write(byte* data, size_t size);
	size_t writeBit(const Bit& bit);
	
	void rewind();

	byte* buffer;
	byte* ptr;
	const size_t capacity;
	size_t realSize;
	FILE* file;
	size_t bitOffset;
};

class FileWriteBuffer
	:public IOBuffer
{
public:
	FileWriteBuffer(const char* fn, size_t bufferSize = 1024* 1024 * 1024);
	~FileWriteBuffer();

	size_t write(byte* data, size_t size);
	size_t writeBit(const Bit& bit);

	BufferData read(size_t size);
	Bit readBit(size_t size);

	byte* buffer;
	byte* ptr;
	const size_t capacity;
	FILE* file;
	size_t byteOffset;
};

#endif