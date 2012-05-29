#ifndef _COMMON_H
#define _COMMON_H

#define HFMZ_VER 0x00000001

typedef unsigned char byte;

struct Bit{
	typedef long BitsType;

	Bit()
		:bits(0), len(0)
	{}

	Bit(const Bit& bit) {
		set(bit);
	}

	BitsType bits;
	size_t len;

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

	void add(BitsType bits, size_t size) {

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

#endif

