#ifndef	GRAPH_H
#define GRAPH_H

#include "Vector3.h"
#include <vector>

struct Edge
{
	unsigned from;
	unsigned to;
	float cost;
};

struct Node
{
	Vector3 pos;
	std::vector<unsigned> edges;
};

class Graph
{
public:
	Graph();
	~Graph();

	void AddEdge(unsigned from, unsigned to);
	void AddNode(Vector3 pos);
	void Generate(unsigned key, unsigned size);

	std::vector<Node*> m_nodes;
	std::vector<Edge*> m_edges;
};

#endif