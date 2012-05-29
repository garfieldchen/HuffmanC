#include "Decoder.h"

#include <algorithm>
using std::sort;

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
	sort(codes, codes + 256);

	return decode(reader, writer, codes, 256);		
}

int HuffmanDecoder::decode(IOReader& reader, IOWriter& writer, BitCode codes[], size_t codeCnt) {
	const size_t minCodeLength = codes[0].bit.len;

	while(true) {
		size_t codeLength = minCodeLength;
		Bit data = reader.readBits(codeLength);
		if (data.len)
			return 0;

		bool find = false;

		for (size_t i = 0; i< codeCnt; ++i ) {
			const Bit& bit = codes[i].bit;
			if (bit.len > codeLength) {
				Bit appendBits = reader.readBits( bit.len - codeLength);

				if (appendBits.len != bit.len - codeLength)
					return 1;

				data.add(appendBits);
				data.len = bit.len;
			}

			if (bit.bits == data.bits) {
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