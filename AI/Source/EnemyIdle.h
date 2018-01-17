#ifndef	IDLE_ENEMY_H
#define IDLE_ENEMY_H

#include "State.h"
#include "GameObject.h"

class IdleEnemy : public State
{
private:
	int idleFor;
public:
	IdleEnemy(const std::string &stateID);
	~IdleEnemy();

	virtual void Enter(GameObject *go);
	virtual void Update(double dt, GameObject *go);
	virtual void Exit(GameObject *go);
};

#endif