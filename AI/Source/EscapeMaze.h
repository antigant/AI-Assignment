#ifndef	ESCAPE_MAZE_H
#define ESCAPE_MAZE_H

// State when the AI player will start escaping when exit is found

#include "State.h"
#include "GameObject.h"
#include <queue>

class EscapeMaze : public State
{
public:
	EscapeMaze(const std::string &stateID);
	~EscapeMaze();

	virtual void Enter(GameObject *go);
	virtual void Update(double dt, GameObject *go);
	virtual void Exit(GameObject *go);

private:
	bool BFSLimit(GameObject *go, MazePt end, int limit);
};

#endif