#ifndef SCENE_QUEEN_H
#define SCENE_QUEEN_H

#include "GameObject.h"
#include <vector>
#include "SceneBase.h"

class SceneQueen : public SceneBase
{
public:
	SceneQueen();
	~SceneQueen();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();

	void RenderGO(GameObject *go);
	GameObject* FetchGO(std::string type);

private:
	bool CheckQueen();
	void DFS(int row);
	std::vector<int> m_queen;
	int m_numSolutions;

protected:

	std::vector<GameObject *> m_goList;
	float m_speed;
	float m_worldWidth;
	float m_worldHeight;
};

#endif