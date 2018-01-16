#ifndef	GAME_OBJECT_H
#define GAME_OBJECT_H

#include "Vector3.h"
#include "ObjectBase.h"
#include "Maze.h"

#include <string>
#include <vector>

class State;

class GameObject : public ObjectBase
{
	std::string type;
public:
	GameObject(std::string typeValue);
	virtual ~GameObject();

	virtual bool Handle(Message *message);

	void ResetGameObject();

	// Getter
	//StateMachine *GetStateMachine(void) const;
	Vector3 GetPosition(void) const;
	Vector3 GetVelocity(void) const;
	Vector3 GetScale(void) const;
	bool GetActive(void) const;
	Vector3 GetTarget(void) const;
	float GetMoveSpeed(void) const;
	std::string GetType(void) const;
	bool GetMoveLeft(void) const;
	bool GetMoveRight(void) const;
	bool GetMoveUp(void) const;
	bool GetMoveDown(void) const;
	bool GetFixMoveLeft(void) const;
	bool GetFixMoveRight(void) const;
	bool GetFixMoveUp(void) const;
	bool GetFixMoveDown(void) const;

	// Setter
	//void SetStateMachine(StateMachine *sm);
	void SetPosition(const Vector3 pos);
	void SetVelocity(const Vector3 vel);
	void SetScale(const Vector3 scale);
	void SetActive(const bool active);
	void SetTarget(const Vector3 target);
	void SetMoveSpeed(const float moveSpeed);
	void SetMoveLeft(const bool moveLeft);
	void SetMoveRight(const bool moveRight);
	void SetMoveUp(const bool moveUp);
	void SetMoveDown(const bool moveDown);
	void SetFixMoveLeft(const bool fixMoveLeft);
	void SetFixMoveRight(const bool fixMoveRight);
	void SetAllMovementSame(const bool movement);

	// States
	void SetCurrentState(State *m_currState);
	void SetNextState(State *m_nextState);

	State *GetCurrentState(void);
	State *GetNextState(void);

	// Assignment2
	void SetMyTurn(const bool myTurn);
	bool GetMyTurn() const;

//private:
	// For lab 8 practical
	std::vector<Maze::TILE_CONTENT> grid;
	std::vector<bool> visited;
	std::vector<MazePt> stack;
	std::vector<MazePt> path;
	MazePt curr;

protected:
	//StateMachine *sm; // Not gonna need this here if gonna be doing share state machine
	Vector3 pos;
	Vector3 vel;
	Vector3 scale;
	bool active;
	Vector3 target;
	float moveSpeed;

	// States
	State *m_currState;
	State *m_nextState;

	// Movements
	bool moveLeft, fixMoveLeft;
	bool moveRight, fixMoveRight;
	bool moveUp, fixMoveUp;
	bool moveDown, fixMoveDown;

	// Assignment 2
	bool myTurn;
};

#endif