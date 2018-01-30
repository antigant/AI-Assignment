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

struct QueueNode // for priority queue
{
	unsigned node;
	float cost;

	QueueNode(unsigned node = 0, float cost = 0) : node(node), cost(cost) {}
};

class Graph
{
public:
	Graph();
	~Graph();

	unsigned NearestNode(const Vector3 &pos);

	void AddEdge(unsigned from, unsigned to);
	void AddNode(Vector3 pos);
	void Generate(unsigned key, unsigned size);

	std::vector<Node*> m_nodes;
	std::vector<Edge*> m_edges;
};

#endif