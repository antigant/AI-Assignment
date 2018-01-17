#ifndef	ENEMY_WONDER_H
#define ENEMY_WONDER_H

#include "State.h"
#include "GameObject.h"
#include <queue>

class EnemyWonder : public State
{
	int factor;
	int steps;
	int wonderFor;
	int changeState;

	bool BFSLimit(GameObject *go, MazePt end, int limit);

public:
	EnemyWonder(const std::string &stateID);
	~EnemyWonder();

	virtual void Enter(GameObject *go);
	virtual void Update(double dt, GameObject *go);
	virtual void Exit(GameObject *go);
};

#endif