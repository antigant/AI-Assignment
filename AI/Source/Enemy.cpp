#include "Enemy.h"
#include "SceneData.h"

Enemy::Enemy(std::string typeValue)
	: GameObject(typeValue)
	, numberOfTurns(0)
{

}

Enemy::~Enemy()
{

}

bool Enemy::Handle(Message *message)
{
	if (message->GetMessageFrom() == "Player")
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

// Setter
void Enemy::SetNumberOfTurns(const int numberOfTurns)
{
	this->numberOfTurns = numberOfTurns;
}

// Getter
int Enemy::GetNumberOfTurns() const
{
	return numberOfTurns;
}

void Enemy::SeeEntireMaze()
{
	for (int i = 0; i < SceneData::GetInstance()->GetNoGrid() * SceneData::GetInstance()->GetNoGrid(); ++i)
		grid[i] = SceneData::GetInstance()->m_maze.See(i);
}

void Enemy::AddNumberOfTurns(const int value)
{
	numberOfTurns += value;
}