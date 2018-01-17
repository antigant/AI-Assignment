#include "Finding.h"
#include "StateMachineManager.h"
#include "SceneData.h"

#include "Player.h"

Finding::Finding(const std::string &stateID)
	: State(stateID)
{

}

Finding::~Finding()
{

}

void Finding::Enter(GameObject *go)
{

}

void Finding::Update(double dt, GameObject *go)
{
	Player *player = dynamic_cast<Player*>(go);
	// Turn base
	if (!go->GetMyTurn())
		return;

	// For DFS
	if (!go->stack.empty())
	{
		// Maze reading state
		DFSOnce(go);
		go->SetMyTurn(false);
	}
	// Do another check to switch state
	if (player->GetExitFound())
		StateMachineManager::GetInstance()->SetNextState(player, "Escape");
}

void Finding::Exit(GameObject *go)
{

}

void Finding::DFSOnce(GameObject *go)
{
	go->visited[go->curr.y * SceneData::GetInstance()->GetNoGrid() + go->curr.x] = true;
	go->stack.push_back(go->curr);
	MazePt next;

	// Check up
	next.Set(go->curr.x, go->curr.y + 1);
	if (next.y < SceneData::GetInstance()->GetNoGrid() && !go->visited[next.y * SceneData::GetInstance()->GetNoGrid() + next.x])
	{
		go->grid[next.y * SceneData::GetInstance()->GetNoGrid() + next.x] = SceneData::GetInstance()->m_maze.See(next);
		if (go->grid[next.y * SceneData::GetInstance()->GetNoGrid() + next.x] == Maze::TILE_EMPTY || go->grid[next.y * SceneData::GetInstance()->GetNoGrid() + next.x] == Maze::TILE_DOOR)
		{
			go->curr = next;
			return;
		}
	}

	// Check down
	next.Set(go->curr.x, go->curr.y - 1);
	if (next.y >= 0 && !go->visited[next.y * SceneData::GetInstance()->GetNoGrid() + next.x])
	{
		go->grid[next.y * SceneData::GetInstance()->GetNoGrid() + next.x] = SceneData::GetInstance()->m_maze.See(next);
		if (go->grid[next.y * SceneData::GetInstance()->GetNoGrid() + next.x] == Maze::TILE_EMPTY || go->grid[next.y * SceneData::GetInstance()->GetNoGrid() + next.x] == Maze::TILE_DOOR)
		{
			go->curr = next;
			return;
		}
	}

	// Check left
	next.Set(go->curr.x - 1, go->curr.y);
	if (next.x >= 0 && !go->visited[next.y * SceneData::GetInstance()->GetNoGrid() + next.x])
	{
		go->grid[next.y * SceneData::GetInstance()->GetNoGrid() + next.x] = SceneData::GetInstance()->m_maze.See(next);
		if (go->grid[next.y * SceneData::GetInstance()->GetNoGrid() + next.x] == Maze::TILE_EMPTY || go->grid[next.y * SceneData::GetInstance()->GetNoGrid() + next.x] == Maze::TILE_DOOR)
		{
			go->curr = next;
			return;
		}
	}

	// Check right
	next.Set(go->curr.x + 1, go->curr.y);
	if (next.x < SceneData::GetInstance()->GetNoGrid() && !go->visited[next.y * SceneData::GetInstance()->GetNoGrid() + next.x])
	{
		go->grid[next.y * SceneData::GetInstance()->GetNoGrid() + next.x] = SceneData::GetInstance()->m_maze.See(next);
		if (go->grid[next.y * SceneData::GetInstance()->GetNoGrid() + next.x] == Maze::TILE_EMPTY || go->grid[next.y * SceneData::GetInstance()->GetNoGrid() + next.x] == Maze::TILE_DOOR)
		{
			go->curr = next;
			return;
		}
	}

	go->stack.pop_back();
	if (!go->stack.empty())
	{
		go->curr = go->stack.back();
		go->stack.pop_back();
		return;
	}
}