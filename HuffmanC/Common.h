#ifndef _COMMON_H
#define _COMMON_H

#define HFMZ_VER 0x00000001

typedef unsigned char byte;

struct Bit{
	typedef unsigned long BitsType;
	enum {BIT_SIZE = sizeof(BitsType) * 8};

	Bit()
		:bits(0), len(0)
	{}

	Bit(const Bit& bit) {
		set(bit);
	}

	BitsType bits;
	size_t len;

	void addBit(int b) {
		assert(len < BIT_SIZE);
		if (b)
			bits |= 0x1 << len;

		++len; 
	}

	void set(const Bit& bit) {
		bits = bit.bits;
		len = bit.len;
	}

	void add(const Bit& bit) {
		assert(len + bit.len <= BIT_SIZE);

		add(bit.bits, bit.len);
	}

	void add(BitsType bits, size_t size) {
		assert(len + size <= BIT_SIZE);


		bits = bits & (~(-1 << size));
		if (len > 0)
			bits <<= len;
		this->bits |= bits;
		len += size;
	}

	bool operator<(Bit& b) {
		return len < b.len;
	}
};


struct HuffmanFileHeader{
	int version;
	int dummyBits;
	Bit codes[256];
};

#include <iostream>
using namespace std;

void printCodes(const Bit bit[], size_t count);

#endif

