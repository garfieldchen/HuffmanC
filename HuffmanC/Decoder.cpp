#include "Decoder.h"

#include <algorithm>
using std::sort;

#include <iostream>
using std::cout;
using std::endl;

void printCodes(const Bit bit[], size_t count)
{
	BitCode codes[256];
	for (size_t i = 0; i < count; ++i) {
		codes[i].bit = bit[i];
		codes[i].value = i;
	}

	sort(codes, codes + count);
	for (size_t i = 0; i < count; ++i)
		cout << (int)codes[i].value << "  " << codes[i].bit.bits << "  :  " << codes[i].bit.len << endl;
}

int HuffmanDecoder::decompress(IOReader& reader, IOWriter& writer) {
	HuffmanFileHeader header;

	BufferData data = reader.read(sizeof(HuffmanFileHeader));
	if (data.size != sizeof(HuffmanFileHeader))
		return 1;
	memcpy(&header, data.buffer, data.size);
	BitCode codes[256];
	for (size_t i = 0; i< 256; ++i) {
		codes[i].bit  = ((Bit*)data.buffer)[i];
		codes[i].value  = i;
	}

	cout << "************************************" << endl;
	printCodes(header.codes, 256);
	sort(codes, codes + 256);

	return decode(reader, writer, codes, 256, header.dummyBits);		
}

int HuffmanDecoder::decode(IOReader& reader, IOWriter& writer, BitCode codes[], size_t codeCnt, size_t excessBit) {
	const size_t fileSize = reader.size() - sizeof(HuffmanFileHeader);

	size_t firstCode = 0;
	while (firstCode < codeCnt && codes[firstCode].bit.len == 0)
		++firstCode;
	if (firstCode >= codeCnt)
		return 0;

	const size_t minCodeLength = codes[firstCode].bit.len;

	const __int64 availableBits = excessBit ? ((fileSize -1 ) * 8 + excessBit) : fileSize * 8;
	__int64 consumeBits = 0;

	cout << "decoding ....... "  << endl;

	while(consumeBits < availableBits) {
		size_t codeLength = minCodeLength;
		Bit data = reader.readBits(codeLength);
		if (data.len != codeLength)
			return 1;

		consumeBits += data.len;

		bool find = false;

		for (size_t i = firstCode; i< codeCnt; ++i ) {
			const Bit& bit = codes[i].bit;
			if (bit.len > codeLength) {
				Bit appendBits = reader.readBits( bit.len - codeLength);

				if (appendBits.len != bit.len - codeLength)
					return 1;

				consumeBits += appendBits.len;

				data.add(appendBits);

				codeLength = bit.len;
			}			

			if (bit.bits == data.bits) {
				cout << bit.len << "  : " << bit.bits << endl;
				writer.write(&(codes[i].value), 1);
				find = true;
				break;
			}
		}

		if (!find )
			return 2;
	};

	return 0;	
}