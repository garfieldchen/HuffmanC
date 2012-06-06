#include "Encoder.h"

bool HuffmanEncoder::compress(IOReader& reader, IOWriter& writer) {
	srcSize = 0;
	compressSize = 0;

	Bit codes[256];
	if (!code(reader, codes))
		return false;
	printCodes(codes, 256);

	writeFile(reader, writer, codes);
	return true;
}

void HuffmanEncoder::writeFile(IOReader& reader, IOWriter& writer, Bit codes[]) {
	reader.rewind();
		
	HuffmanFileHeader header;
	header.version = 0x00000001;
	header.dummyBits = 0;
	memcpy(header.codes, codes, sizeof(header.codes));
	writer.write((const byte*)&header, sizeof(HuffmanFileHeader));

	cout << "writing ....." << endl;
	BufferData buff = reader.read(1);
	while (buff.size == 1) {
		writer.writeBit(codes[((int)(*buff.buffer))]);
		Bit& bit = codes[((int)(*buff.buffer))];
		cout << bit.len << "  : " << bit.bits << endl;
		buff = reader.read(1);
	}

	header.dummyBits = writer.bitPosition();
	writer.rewind();
	writer.write((byte*)&header, sizeof(HuffmanFileHeader));
}

bool HuffmanEncoder::code(IOReader& ioBuffer, Bit codes[]) {
	long stats[256];
	memset(stats, 0, sizeof(long) * 256);

	BufferData bufferData = ioBuffer.read(1);

	while(bufferData.size == 1) {
		++ stats[*bufferData.buffer];
		bufferData = ioBuffer.read(1);

		++srcSize;
	}

	priority_queue<Node*,vector<Node*>, NodePtrCompare> q;
	for (size_t i = 0; i < 256; ++i) {
		if ( stats[i] >0 )
			q.push(new Node(NULL, NULL, stats[i], i));
	}

	while(q.size() > 1) {
		Node* n1 = q.top();
		q.pop();
		Node* n2 = q.top();
		q.pop();
			
		q.push( new Node(n1, n2, n1->weight + n2->weight));
	}

	if (q.empty())
		return false;

	queue<Node*> unvisitNodes;
	unvisitNodes.push(q.top());

	while (!unvisitNodes.empty()) {
		Node* node = unvisitNodes.front();
 		unvisitNodes.pop();

#define PUSH_NODE_BIT(n, b) do { \
	if (n) { \
		n->bit.set(node->bit); \
		n->bit.addBit(b);	\
		unvisitNodes.push(n); \
	} \
} while(false);

		PUSH_NODE_BIT(node->left, 0);
		PUSH_NODE_BIT(node->right, 1);

		if (!node->left && !node->right) {
			codes[node->value] = node->bit;
			compressSize += node->bit.len * node->weight / 8;
			//cout << node->value << " : " << node->bit.len << " " << node->bit.bits << "  " << node->weight << endl;
		}
	}
	return true;
}