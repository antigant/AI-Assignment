#include "EnemyIdle.h"
#include "StateMachineManager.h"
#include "SceneData.h"
#include "PostOffice.h"
#include "Message.h"

#include "Enemy.h"

EnemyIdle::EnemyIdle(const std::string &stateID)
	: State(stateID)
{

}

EnemyIdle::~EnemyIdle()
{

}

void EnemyIdle::Enter(GameObject *go)
{
	idleFor = Math::RandIntMinMax(0, 5);
}

void EnemyIdle::Update(double dt, GameObject *go)
{
	if (!go->GetMyTurn())
		return;

	Enemy *enemy = dynamic_cast<Enemy*>(go);
	if (enemy->GetNumberOfTurns() >= idleFor)
	{
		StateMachineManager::GetInstance()->SetNextState(enemy, "Wonder");
		enemy->SetNumberOfTurns(0);
	}

	enemy->AddNumberOfTurns(1);
	go->SetMyTurn(false);
	PostOffice::GetInstance()->Send("Player", new Message("Enemy", "Your turn"));
}

void EnemyIdle::Exit(GameObject *go)
{

}