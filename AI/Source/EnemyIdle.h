#ifndef	IDLE_ENEMY_H
#define IDLE_ENEMY_H

#include "State.h"
#include "GameObject.h"

class EnemyIdle : public State
{
private:
	int idleFor;
public:
	EnemyIdle(const std::string &stateID);
	~EnemyIdle();

	virtual void Enter(GameObject *go);
	virtual void Update(double dt, GameObject *go);
	virtual void Exit(GameObject *go);
};

#endif