#ifndef	FINDING_H
#define FINDING_H

// Player state where he will try to find the exit and run away
// Use DFS to search the maze

#include "State.h"
#include "GameObject.h"

class Finding : public State
{
public:
	Finding(const std::string &stateID);
	~Finding();

	virtual void Enter(GameObject *go);
	virtual void Update(double dt, GameObject *go);
	virtual void Exit(GameObject *go);

private:
	void DFSOnce(GameObject *go);
};

#endif