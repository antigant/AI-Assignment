#include "Player.h"

Player::Player(std::string typeValue, bool isAI)
	: GameObject(typeValue)
	, isAI(isAI)
	, exitFound(false)
	, exitPt(MazePt(0.f, 0.f))
{
	myTurn = true;
}

Player::~Player()
{

}

bool Player::Handle(Message *message)
{
	if (message->GetMessageFrom() == "Enemy")
	{
		if (message->GetMessageType() == "Your turn")
		{
			SetMyTurn(true);
			delete message;
			return true;
		}
	}

	delete message;
	return false;
}

void Player::SetIsAI(const bool isAI)
{
	this->isAI = isAI;
}

void Player::SetExitFound(const bool exitFound)
{
	this->exitFound = exitFound;
}

void Player::SetExitPt(const MazePt exitPt)
{
	this->exitPt = exitPt;
}

bool Player::GetIsAI() const
{
	return isAI;
}

bool Player::GetExitFound() const
{
	return exitFound;
}

MazePt Player::GetExitPt() const
{
	return exitPt;
}