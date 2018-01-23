#include "Graph.h"

Graph::Graph()
{

}

Graph::~Graph()
{
	while (m_nodes.back())
	{
		delete m_nodes.back();
		m_nodes.pop_back();
	}
}

unsigned Graph::NearestNode(const Vector3 &pos)
{
	unsigned nearest = 0;
	if (m_nodes.size() != 0)
	{
		float distance = (m_nodes[0]->pos - pos).Length();
		for (int i = 1; i < m_nodes.size(); ++i)
		{
			float temp = (m_nodes[i]->pos - pos).Length();
			if (temp < distance)
			{
				distance = temp;
				nearest = i;
			}
		}
	}
	return nearest;
}

void Graph::AddEdge(unsigned from, unsigned to)
{
	Edge *newEdge = new Edge;
	newEdge->from = from;
	newEdge->to = to;

	if (from < m_nodes.size() || to < m_nodes.size())
	{
		Vector3 dist = m_nodes[from]->pos - m_nodes[to]->pos;
		newEdge->cost = dist.Length(); 
	}

	// Allocate to related node
	m_nodes[from]->edges.push_back(m_edges.size());

	m_edges.push_back(newEdge);
}

void Graph::AddNode(Vector3 pos)
{
	Node *newNode = new Node;
	newNode->pos = pos;
	m_nodes.push_back(newNode);
}

void Graph::Generate(unsigned key, unsigned size)
{
	AddNode(Vector3(10, 10));
	AddNode(Vector3(80, 10));
	AddNode(Vector3(10, 80));
	AddNode(Vector3(80, 80));
	AddEdge(0, 1);
	AddEdge(1, 0);
	AddEdge(0, 2);
	AddEdge(2, 0);
	AddEdge(1, 3);
	AddEdge(3, 1);
	AddEdge(2, 3);
	AddEdge(3, 2);
}