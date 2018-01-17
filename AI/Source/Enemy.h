#ifndef	ENEMY_H
#define ENEMY_H

#include "GameObject.h"

class Enemy : public GameObject
{
private:
	int numberOfTurns;

public:
	Enemy(std::string typeValue);
	~Enemy();

	virtual bool Handle(Message *message);

	// Setter
	void SetNumberOfTurns(const int numberOfTurns);
	// Getter
	int GetNumberOfTurns() const;

	void AddNumberOfTurns(const int value);
};

#endif