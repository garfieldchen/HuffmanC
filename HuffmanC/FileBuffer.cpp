#include "FileBuffer.h"
#include <string.h>
#include <assert.h>
#include <math.h>
#include <memory.h>
#include <algorithm>

FileReadBuffer::FileReadBuffer(const char* fn, size_t bufferSize) 
	:capacity(bufferSize)
{
	buffer = (byte*)malloc(bufferSize);
	ptr = buffer;
	realSize = 0;
	bitOffset = 0;

	file = fopen(fn, "rb");
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

	ptr += size;
	return BufferData(ptr, size);
}

Bit FileReadBuffer::readBits(size_t size)
{
	Bit retBit;

	if (bitOffset>0) {
		const size_t availableCnt = 8 - bitOffset;
		const size_t usedCnt = std::min(size, availableCnt);
		
		retBit.add((long)(*ptr) >> bitOffset, usedCnt);
		bitOffset = (bitOffset + usedCnt) % 8;
		size -= bitOffset;

		if (size <= availableCnt)
			return retBit;	
	}

	const size_t byteCnt = size / 8 + (size % 8 ? 1 : 0); 
	BufferData data = read(byteCnt);
	if (data.size < byteCnt)
		return Bit();
	bitOffset = size % 8;
	
	retBit.add(*((long*)data.buffer), byteCnt); // TODO there will be a bug, read invalid address
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
	buffer = (byte*)malloc(capacity);
	file = fopen(fn, "wb+");
	ptr = buffer;
	bitOffset = 0;
}

FileWriteBuffer::~FileWriteBuffer() {
	if (ptr != buffer)
		fwrite(buffer, ptr - buffer, 1, file);

	free(buffer);
	fclose(file);
}

size_t FileWriteBuffer::write(const byte* data, size_t size) {
	assert(size <= capacity);
	size_t copySize = std::min(size, capacity - (ptr - buffer));
	
	memcpy(ptr, data, copySize);
	if (ptr == (buffer + capacity)) {
		fwrite(buffer, capacity, 1, file);
		ptr = buffer;
	}

	const size_t restSize = size - copySize;
	if (restSize > 0) {
		memcpy(buffer, data + copySize, restSize);
		ptr = buffer + restSize;
	}
	return size;
}

void FileWriteBuffer::rewind() {
	fseek(file, 0, SEEK_SET);
	ptr = buffer;
	bitOffset = 0;
}

size_t FileWriteBuffer::writeBit(const Bit& bit)
{
	return 0;
}
