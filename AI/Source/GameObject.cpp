#include "GameObject.h"
#include "State.h"

GameObject::GameObject(std::string typeValue)
	: type(typeValue)
	, scale(1.f, 1.f, 1.f)
	, active(false)
	, moveSpeed(1.f)
	, m_currState(NULL)
	, m_nextState(NULL)
	, moveLeft(false)
	, moveRight(false)
	, moveUp(false)
	, moveDown(false)
	// Assignment2
	, myTurn(false)
{

}

GameObject::~GameObject()
{
	//if (sm != NULL)
	//{
	//	delete sm;
	//	sm = NULL;
	//}
}

bool GameObject::Handle(Message *message)
{
	delete message;
	return false;
}

void GameObject::ResetGameObject()
{
	//if (sm != NULL)
	//{
	//	delete sm;
	//	sm = NULL;
	//}
/*
	scale.Set(1.f, 1.f, 1.f);*/
	active = false;
	//moveSpeed = 1.f;
	moveLeft = moveRight = moveUp = moveDown = false;
}

//// Getter
//void GameObject::SetStateMachine(StateMachine *sm)
//{
//	this->sm = sm;
//}
//
//StateMachine *GameObject::GetStateMachine(void) const
//{
//	return sm;
//}

Vector3 GameObject::GetPosition(void) const
{
	return pos;
}

Vector3 GameObject::GetVelocity(void) const
{
	return vel;
}

Vector3 GameObject::GetScale(void) const
{
	return scale;
}

bool GameObject::GetActive(void) const
{
	return active;
}

Vector3 GameObject::GetTarget(void) const
{
	return target;
}

float GameObject::GetMoveSpeed(void) const
{
	return moveSpeed;
}

std::string GameObject::GetType(void) const
{
	return type;
}

bool GameObject::GetMoveLeft(void) const
{
	return moveLeft;
}

bool GameObject::GetMoveRight(void) const
{
	return moveRight;
}

bool GameObject::GetMoveUp(void) const
{
	return moveUp;
}

bool GameObject::GetMoveDown(void) const
{
	return moveDown;
}

bool GameObject::GetFixMoveLeft(void) const
{
	return fixMoveLeft;
}

bool GameObject::GetFixMoveRight(void) const
{
	return fixMoveRight;
}

bool GameObject::GetFixMoveUp(void) const
{
	return fixMoveUp;
}

bool GameObject::GetFixMoveDown(void) const
{
	return fixMoveDown;
}

// Setter
void GameObject::SetPosition(const Vector3 pos)
{
	this->pos = pos;
}

void GameObject::SetVelocity(const Vector3 vel)
{
	this->vel = vel;
}

void GameObject::SetScale(const Vector3 scale)
{
	this->scale = scale;
}

void GameObject::SetActive(const bool active)
{
	this->active = active;
}

void GameObject::SetTarget(const Vector3 target)
{
	this->target = target;
}

void GameObject::SetMoveSpeed(const float moveSpeed)
{
	this->moveSpeed = moveSpeed;
}

void GameObject::SetMoveLeft(const bool moveLeft)
{
	this->moveLeft = moveLeft;
}

void GameObject::SetMoveRight(const bool moveRight)
{
	this->moveRight = moveRight;
}

void GameObject::SetMoveUp(const bool moveUp)
{
	this->moveUp = moveUp;
}

void GameObject::SetMoveDown(const bool moveDown)
{
	this->moveDown = moveDown;
}

void GameObject::SetFixMoveLeft(const bool fixMoveLeft)
{
	this->fixMoveLeft = fixMoveLeft;
}

void GameObject::SetFixMoveRight(const bool fixMoveRight)
{
	this->fixMoveRight = fixMoveRight;
}

void GameObject::SetAllMovementSame(const bool movement)
{
	moveLeft = moveRight = moveUp = moveDown = movement;
	fixMoveLeft = fixMoveRight = fixMoveUp = fixMoveDown = movement;
}

void GameObject::SetCurrentState(State *m_currState)
{
	this->m_currState = m_currState;
}

void GameObject::SetNextState(State *m_nextState)
{
	this->m_nextState = m_nextState;
}

State *GameObject::GetCurrentState(void)
{
	return m_currState;
}

State *GameObject::GetNextState(void)
{
	return m_nextState;
}

// Assignment 2
void GameObject::SetMyTurn(const bool myTurn)
{
	this->myTurn = myTurn;
}

bool GameObject::GetMyTurn() const
{
	return myTurn;
}

void GameObject::SetFaction(const std::string faction)
{
	this->faction = faction;
}

std::string GameObject::GetFaction(void) const
{
	return faction;
}
