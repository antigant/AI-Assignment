#include "EnemyIdle.h"
#include "StateMachineManager.h"
#include "SceneData.h"
#include "PostOffice.h"
#include "Message.h"

#include "Enemy.h"

IdleEnemy::IdleEnemy(const std::string &stateID)
	: State(stateID)
{

}

IdleEnemy::~IdleEnemy()
{

}

void IdleEnemy::Enter(GameObject *go)
{
	idleFor = Math::RandIntMinMax(0, 5);
}

void IdleEnemy::Update(double dt, GameObject *go)
{
	Enemy *enemy = dynamic_cast<Enemy*>(go);

	if (!go->GetMyTurn())
		return;

	go->SetMyTurn(false);
	if (enemy->GetNumberOfTurns() != idleFor)
	{
		enemy->AddNumberOfTurns(1);
		PostOffice::GetInstance()->Send("Player", new Message("Enemy", "Your turn"));
		return;
	}

	StateMachineManager::GetInstance()->SetNextState(enemy, "Wonder");
}

void IdleEnemy::Exit(GameObject *go)
{

}