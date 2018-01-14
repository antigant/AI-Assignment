#ifndef	PLAYER_H
#define PLAYER_H

#include "GameObject.h"

class Player : public GameObject
{
private:
	bool isAI; // Check if the player is AI or human controlled
	bool exitFound;
	MazePt exitPt;

public:
	Player(std::string typeValue, bool isAI = true);
	~Player();

	void SetIsAI(const bool isAI);
	void SetExitFound(const bool exitFound);
	void SetExitPt(const MazePt exitPt);

	bool GetIsAI() const;
	bool GetExitFound() const;
	MazePt GetExitPt() const;
};

#endif