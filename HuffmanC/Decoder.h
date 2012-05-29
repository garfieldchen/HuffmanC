#ifndef _DECODER_H
#define _DECODER_H

#include "FileBuffer.h"

struct BitCode{
	Bit bit;
	byte value;
	bool operator<(BitCode& bc) { return bit.len < bc.bit.len; }
};

class HuffmanDecoder 
{
public:
	int decompress(IOReader& reader, IOWriter& writer);
private:
	void sortCodes(Bit codes[], int indexes[], size_t count);
	int decode(IOReader& reader, IOWriter& writer, BitCode codes[], size_t codeCnt);
};
#endif