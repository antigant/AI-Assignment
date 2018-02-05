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
	//AddNode(Vector3(10, 10));
	//AddNode(Vector3(80, 10));
	//AddNode(Vector3(10, 80));
	//AddNode(Vector3(80, 80));
	//AddEdge(0, 1);
	//AddEdge(1, 0);
	//AddEdge(0, 2);
	//AddEdge(2, 0);
	//AddEdge(1, 3);
	//AddEdge(3, 1);
	//AddEdge(2, 3);
	//AddEdge(3, 2);

	// Radiant
	// Radiant ancient
	AddNode(Vector3(5.f, 5.f));
	// top lane
	AddNode(Vector3(5.f, 40.f));
	AddNode(Vector3(5.f, 75.f));
	// mid lane
	AddNode(Vector3(25.f, 25.f));
	AddNode(Vector3(42.5f, 42.5f));
	// btm lane
	AddNode(Vector3(40.f, 5.f));
	AddNode(Vector3(75.f, 5.f));

	// Dire
	// top lane
	AddNode(Vector3(25.f, 95.f));
	AddNode(Vector3(60.f, 95.f));
	// mid lane
	AddNode(Vector3(57.5f, 57.5f));
	AddNode(Vector3(75.f, 75.f));

	// btm lane
	AddNode(Vector3(95.f, 60.f));
	AddNode(Vector3(95.f, 25.f));
	// Dire ancient
	AddNode(Vector3(95.f, 95.f));

	// Top lane (Radiant)
	AddEdge(0, 1);
	AddEdge(1, 2);
	AddEdge(2, 7);
	AddEdge(7, 8);
	AddEdge(8, 13);

	// Mid lane (Radiant)
	AddEdge(0, 3);
	AddEdge(3, 4);
	AddEdge(4, 9);
	AddEdge(9, 10);
	AddEdge(10, 13);

	// Btm lane (Radiant)
	AddEdge(0, 5);
	AddEdge(5, 6);
	AddEdge(6, 12);
	AddEdge(12, 11);
	AddEdge(11, 13);

	// Top lane (Dire)
	AddEdge(13, 8);
	AddEdge(8, 7);
	AddEdge(7, 2);
	AddEdge(2, 1);
	AddEdge(1, 0);

	// Mid lane (Dire)
	AddEdge(13, 10);
	AddEdge(10, 9);
	AddEdge(9, 4);
	AddEdge(4, 3);
	AddEdge(3, 0);

	// Btm lane (Dire)
	AddEdge(13, 11);
	AddEdge(11, 12);
	AddEdge(12, 6);
	AddEdge(6, 5);
	AddEdge(5, 0);
}