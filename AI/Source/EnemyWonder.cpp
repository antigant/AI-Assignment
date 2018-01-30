#include "EnemyWonder.h"
#include "StateMachineManager.h"
#include "SceneData.h"
#include "PostOffice.h"
#include "Message.h"

#include "Enemy.h"

EnemyWonder::EnemyWonder(const std::string &stateID)
	: State(stateID)
{

}

EnemyWonder::~EnemyWonder()
{

}

void EnemyWonder::Enter(GameObject *go)
{
	// In this state for how many turns
	wonderFor = Math::RandIntMinMax(1, 4);
	// Decide to either go back to idle state of go back to rampage state
	changeState = Math::RandIntMinMax(0, 2);

	MazePt next;
	// Go left
	if (factor == 0)
	{
		if (go->curr.x > 0)
			next.Set(go->curr.x - steps, go->curr.y);
		else
			next.Set(go->curr.x + steps, go->curr.y);
	}
	// Go right
	else if (factor == 1)
	{
		if (go->curr.x < SceneData::GetInstance()->GetNoGrid())
			next.Set(go->curr.x + steps, go->curr.y);
		else
			next.Set(go->curr.x - steps, go->curr.y);
	}
	// Go up
	else if (factor == 2)
	{
		if (go->curr.y < SceneData::GetInstance()->GetNoGrid())
			next.Set(go->curr.x, go->curr.y + steps);
		else
			next.Set(go->curr.x, go->curr.y - steps);
	}
	else if (factor == 3)
	{
		if (go->curr.y > 0)
			next.Set(go->curr.x, go->curr.y - steps);
		else
			next.Set(go->curr.x, go->curr.y + steps);
	}

	BFSLimit(go, next, 10);
}

void EnemyWonder::Update(double dt, GameObject *go)
{
	if (!go->GetMyTurn())
		return;
	Enemy *enemy = dynamic_cast<Enemy*>(go);
	// To decide the direction it will go
	factor = Math::RandIntMinMax(0, 4);
	// Number of steps to walk for
	steps = Math::RandIntMinMax(2, 4);

	//if (!go->path.empty())
	//{
	//	// Path finding state
	//	go->curr = go->path[0];
	//	go->path.erase(go->path.begin());
	//}

	enemy->AddNumberOfTurns(1);
	go->SetMyTurn(false);
	PostOffice::GetInstance()->Send("Player", new Message("Enemy", "Your turn"));

	if (enemy->GetNumberOfTurns() >= wonderFor)
	{
		//if(changeState == 0)
		StateMachineManager::GetInstance()->SetNextState(enemy, "Idle");
		//else if(changeState == 1)
		//	StateMachineManager::GetInstance()->SetNextState(enemy, "Rampage");

		enemy->SetNumberOfTurns(0);
	}
}

void EnemyWonder::Exit(GameObject *go)
{

}

bool EnemyWonder::BFSLimit(GameObject *go, MazePt end, int limit)
{
	std::vector<bool> visited(SceneData::GetInstance()->GetNoGrid() * SceneData::GetInstance()->GetNoGrid(), false);
	std::queue<MazePt> queue;
	std::vector<MazePt> previous(SceneData::GetInstance()->GetNoGrid() * SceneData::GetInstance()->GetNoGrid());
	queue.push(go->curr);
	int nearestDistance = INT_MAX;
	MazePt nearestTile = go->curr;
	MazePt curr;

	int loop = 0;

	while (!queue.empty() && loop < limit)
	{
		++loop;
		curr = queue.front();
		queue.pop();

		int distance = abs(curr.x - end.x) + abs(curr.y - end.y);
		if (distance < nearestDistance)
		{
			nearestDistance = distance;
			nearestTile = curr;
		}

		if (curr.x == end.x && curr.y == end.y)
		{
			while (curr.x != go->curr.x || curr.y != go->curr.y)
			{
				go->path.insert(go->path.begin(), curr);
				curr = previous[curr.y * SceneData::GetInstance()->GetNoGrid() + curr.x];
			}
			go->path.insert(go->path.begin(), curr);
			return true;
		}

		for (int y = 1; y >= -1; --y)
		{
			for (int x = 1; x >= -1; --x)
			{
				if (1 != abs(x - y))
					continue;

				MazePt next(curr.x + x, curr.y + y);

				if (next.x < 0 || next.x >= SceneData::GetInstance()->GetNoGrid() || next.y < 0 || next.y >= SceneData::GetInstance()->GetNoGrid())
					continue;

				if (!visited[next.y * SceneData::GetInstance()->GetNoGrid() + next.x] &&
					(go->grid[next.y * SceneData::GetInstance()->GetNoGrid() + next.x] == Maze::TILE_EMPTY) || (go->grid[next.y * SceneData::GetInstance()->GetNoGrid() + next.x] == Maze::TILE_DOOR))
				{
					previous[next.y * SceneData::GetInstance()->GetNoGrid() + next.x] = curr;
					queue.push(next);
					visited[next.y * SceneData::GetInstance()->GetNoGrid() + next.x] = true;
				}
			}
		}
	}

	curr = nearestTile;

	while (curr.x != go->curr.x || curr.y != go->curr.y)
	{
		go->path.insert(go->path.begin(), curr);
		curr = previous[curr.y * SceneData::GetInstance()->GetNoGrid() + curr.x];
	}
	go->path.insert(go->path.begin(), curr);

	return false;
}