#include "EscapeMaze.h"
#include "StateMachineManager.h"
#include "SceneData.h"
#include "PostOffice.h"
#include "Message.h"

#include "Player.h"

EscapeMaze::EscapeMaze(const std::string &stateID)
	: State(stateID)
{

}

EscapeMaze::~EscapeMaze()
{

}

void EscapeMaze::Enter(GameObject *go)
{

}

void EscapeMaze::Update(double dt, GameObject *go)
{
	if (!go->GetMyTurn())
		return;

	Player *player = dynamic_cast<Player*>(go);
	BFSLimit(go, player->GetExitPt(), 60);

	if (!go->path.empty())
	{
		// Path finding state
		go->curr = go->path[0];
		go->path.erase(go->path.begin());
	}

	go->SetMyTurn(false);
	PostOffice::GetInstance()->Send("Enemy", new Message("Player", "Your turn"));
}

void EscapeMaze::Exit(GameObject *go)
{

}

bool EscapeMaze::BFSLimit(GameObject *go, MazePt end, int limit)
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