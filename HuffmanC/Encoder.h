#include <queue>
#include <vector>
#include <assert.h>
#include "FileBuffer.h"
#include <iostream>

using namespace std;

struct Node {
	Node(Node* l, Node* r, long w, int v = 0)
		:left(l), right(r), value(v), weight(w)
	{}

	Node* left;
	Node* right;
	int value;

	long weight;

	Bit bit;
};

struct NodePtrCompare {
	bool operator()(Node* n1, Node* n2) {
		return n1->weight > n2->weight;
	}
};

class HuffmanEncoder {
public:
	bool compress(IOReader& reader, IOWriter& writer);
private:
	void writeFile(IOReader& reader, IOWriter& writer, Bit codes[]);
	bool code(IOReader& ioBuffer, Bit codes[]);

public:
	size_t srcSize;
	size_t compressSize;
};