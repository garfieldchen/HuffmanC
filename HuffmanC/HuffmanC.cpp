// HuffmanC.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
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

class Huffman {
public:
	bool compress(IOBuffer& reader, IOBuffer& writer) {
		Bit codes[256];
		if (!code(reader, codes))
			return false;

		writeFile(reader, writer, codes);
		return true;
	}

	void writeFile(IOBuffer& reader, IOBuffer& writer, Bit codes[]) {
		reader.rewind();
		
		writer.write((byte*)codes, sizeof(Bit) * 256);
		BufferData buff = reader.read(1);
		while (buff.size == 1)
			writer.writeBit(codes[((int)(*buff.buffer))]);
	}

	bool code(IOBuffer& ioBuffer, Bit codes[]) {
		long stats[256];
		memset(stats, 0, sizeof(long) * 256);

		BufferData bufferData = ioBuffer.read(1);

		while(bufferData.size == 1) {
			stats[*bufferData.buffer] += 1;
			bufferData = ioBuffer.read(1);
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
				cout << node->value << " : " << node->bit.len << " " << node->bit.bits << "  " << node->weight << endl;
			}
		}
		return true;
	}
};

int _tmain(int argc, _TCHAR* argv[])
{
	FileReadBuffer reader("d:\\lua-5.2.0.tar");
	FileReadBuffer writer("d:\\abc.hfmz");
	Huffman compresor;
	compresor.compress(reader, writer);

	return 0;
}

