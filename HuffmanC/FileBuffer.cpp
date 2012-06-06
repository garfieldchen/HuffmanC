#include "FileBuffer.h"
#include <string.h>
#include <assert.h>
#include <math.h>
#include <memory.h>
#include <algorithm>

FileReadBuffer::FileReadBuffer(const char* fn, size_t bufferSize) 
	:capacity(bufferSize)
{	
	realSize = 0;
	bitOffset = 0;

	file = fopen(fn, "rb");
	if (file ) {
		buffer = (byte*)malloc(bufferSize);
		memset(buffer, 0, bufferSize);
	} else
		buffer = 0;

	ptr = buffer;
}

FileReadBuffer::~FileReadBuffer() {
	if (file)
		fclose(file);
	free(buffer);
}

BufferData FileReadBuffer::read(size_t size) {
	if (!file)
		return BufferData();

	const size_t availableSize = (size_t)(realSize - (size_t)(ptr - buffer));
	if (availableSize < size ) {
		if (availableSize > 0 )
			memcpy(buffer, ptr, availableSize);
		size_t readed = fread(buffer + availableSize, 1, capacity - availableSize, file);
		ptr = buffer;
		realSize = readed + availableSize;

		if (realSize < size)
			return BufferData();
	}

	BufferData ret (ptr, size);
	ptr += size;
	return ret;
}

Bit FileReadBuffer::readBits(size_t size)
{
	Bit retBit;

	if (bitOffset>0) {
		const size_t availableCnt = 8 - bitOffset;
		const size_t usedCnt = std::min(size, availableCnt);
		
		retBit.add((long)(*ptr) >> bitOffset, usedCnt);
		bitOffset = (bitOffset + usedCnt) % 8;

		if (size <= availableCnt)
			return retBit;	

		size -= usedCnt;
	}

	const size_t byteCnt = size / 8 + (size % 8 ? 1 : 0); 
	BufferData data = read(byteCnt);
	if (data.size < byteCnt)
		return Bit();
	bitOffset = size % 8;
	
	retBit.add(*((unsigned long*)data.buffer), size); // TODO there will be a bug, read invalid address
	return retBit;
}

void FileReadBuffer::rewind() {
	fseek(file, 0, SEEK_SET);
	ptr = buffer;
	realSize = 0;
	bitOffset = 0;
}

size_t FileReadBuffer::size() {
	size_t pos = ftell(file);
	fseek(file, 0, SEEK_END);
	size_t fileSize = ftell(file);
	if (pos != fileSize)
		fseek(file, pos, SEEK_SET);

	return fileSize;
}
///////////////////////
FileWriteBuffer::FileWriteBuffer(const char* fn, size_t bufferSize)
	:capacity(bufferSize)
{
	file = fopen(fn, "wb+");
	if (file) {
		buffer = (byte*)malloc(capacity);
		memset(buffer, 0, bufferSize);
	} else
		buffer = 0;
	
	ptr = buffer;
	bitOffset = 0;
}

FileWriteBuffer::~FileWriteBuffer() {
	flush();

	free(buffer);
	fclose(file);
}

void FileWriteBuffer::flush() {
	if (ptr != buffer || bitOffset != 0) {
		const size_t size = ptr - buffer + (bitOffset ? 1 : 0);
		fwrite(buffer, size, 1, file);
	}
}

size_t FileWriteBuffer::write(const byte* data, size_t size) {
	assert(size <= capacity);

	size_t copySize = std::min(size, capacity - (ptr - buffer));	
	memcpy(ptr, data, copySize);
	ptr += copySize;

	if (ptr == (buffer + capacity)) {
		fwrite(buffer, capacity, 1, file);
		memset(buffer, 0, capacity);
		ptr = buffer;
	}

	const size_t restSize = size - copySize;
	if (restSize > 0)
		memcpy(buffer, data + copySize, restSize);
	ptr += restSize;

	return size;
}

void FileWriteBuffer::rewind() {
	flush();
	fseek(file, 0, SEEK_SET);
	ptr = buffer;
	bitOffset = 0;
}

size_t FileWriteBuffer::writeBit(const Bit& bit)
{
	Bit::BitsType bits = (bit.bits << bitOffset ) | (*ptr);
	size_t byteCnt = (bit.len + bitOffset) / 8;
	if (byteCnt > 0)
		write((byte*)&bits, byteCnt);
	else
		*ptr = (byte)bits;

	bitOffset = (bit.len + bitOffset) % 8;
	if (bitOffset > 0)
		*ptr = (byte)(bits >> (byteCnt * 8));

	return bit.len;
}
