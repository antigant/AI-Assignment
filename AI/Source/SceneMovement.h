#ifndef SCENE_MOVEMENT_H
#define SCENE_MOVEMENT_H

#include "GameObject.h"
#include <vector>
#include "SceneBase.h"
#include "ObjectBase.h"

class SceneMovement : public SceneBase, public ObjectBase
{
public:
	SceneMovement();
	~SceneMovement();

	virtual void Init();
	virtual void Update(double dt);
	virtual bool Handle(Message *message);
	virtual void Render();
	virtual void Exit();

	void RenderGO(GameObject *go);

	GameObject* FetchGO(std::string type);
protected:
	std::vector<GameObject *> m_goList;
	float m_worldWidth;
	float m_worldHeight;
	GameObject *m_ghost;
	float m_hourOfTheDay;
	float zOffset;

	double SpawnTime, FixSpawnTime;
};

#endif