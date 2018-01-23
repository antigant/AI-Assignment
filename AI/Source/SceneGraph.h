#ifndef SCENE_GRAPH_H
#define SCENE_GRAPH_H

#include "GameObject.h"
#include <vector>
#include "SceneBase.h"
#include "Maze.h"
#include <queue>
#include "Graph.h"

class SceneGraph : public SceneBase
{
public:
	SceneGraph();
	~SceneGraph();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();

	void RenderGO(GameObject *go);
	GameObject* FetchGO(std::string type);

	void RenderGraph();

private:
	void GoRandomAdjacent(GameObject *go);
	void DFSOnce(GameObject *go);
	bool AStar(GameObject *go, unsigned start, unsigned end);

protected:

	std::vector<GameObject *> m_goList;
	float m_speed;
	float m_worldWidth;
	float m_worldHeight;
	int m_objectCount;

	Graph m_graph;
	unsigned m_graphKey;
};

#endif