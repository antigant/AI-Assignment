#include "SceneTurn.h"
#include "GL\glew.h"
#include "Application.h"

#include "SceneData.h"
#include "Player.h"
#include "Enemy.h"

// PostOffice
#include "PostOffice.h"
// States
#include "StateMachineManager.h"
#include "Finding.h"
#include "EscapeMaze.h"
#include "EnemyIdle.h"
#include "EnemyWonder.h"

SceneTurn::SceneTurn()
{
}

SceneTurn::~SceneTurn()
{
}

void SceneTurn::Init()
{
	SceneBase::Init();

	//Calculating aspect ratio
	m_worldHeight = 100.f;
	m_worldWidth = m_worldHeight * (float)Application::GetWindowWidth() / Application::GetWindowHeight();

	//Physics code here
	m_speed = 1.f;

	Math::InitRNG();

	m_noGrid = 12;
	SceneData::GetInstance()->SetNoGrid(m_noGrid);
	m_gridSize = m_worldHeight / m_noGrid;
	m_gridOffset = m_gridSize / 2;

	// Enable fog
	fog = true;

	fogList.resize(m_noGrid * m_noGrid);
	std::fill(fogList.begin(), fogList.end(), Maze::FOG::UNSEEN);

	m_start.Set(0, 0);
	//m_mazeKey = 0;
	//m_wallLoad = 0.25f;
	m_mazeKey = Math::RandIntMinMax(0, 100);
	m_wallLoad = Math::RandFloatMinMax(0.25f, 0.35f);
	SceneData::GetInstance()->m_maze.Generate(m_mazeKey, m_noGrid, m_start, m_wallLoad); //Generate new maze
	m_myGrid.resize(m_noGrid * m_noGrid);
	m_visited.resize(m_noGrid * m_noGrid);
	m_previous.resize(m_noGrid * m_noGrid);
	std::fill(m_myGrid.begin(), m_myGrid.end(), Maze::TILE_FOG);
	std::fill(m_visited.begin(), m_visited.end(), false);
	m_myGrid[m_start.y * m_noGrid + m_start.x] = Maze::TILE_EMPTY;
	//DFS(m_start);

	PostOffice::GetInstance()->Register("Scene", this);

	// Player
	StateMachineManager::GetInstance()->AddState("Player", new Finding("Finding"));
	StateMachineManager::GetInstance()->AddState("Player", new EscapeMaze("Escape"));
	GameObject *go = FetchGO("Player");
	PostOffice::GetInstance()->Register("Player", go);

	// Enemy
	StateMachineManager::GetInstance()->AddState("Enemy", new EnemyIdle("Idle"));
	StateMachineManager::GetInstance()->AddState("Enemy", new EnemyWonder("Wonder"));
	go = FetchGO("Enemy");
	PostOffice::GetInstance()->Register("Enemy", go);

	SeeEnemy = true;
	gameEnded = false;
	gameJustStarted = true;

	m_turn = 0;
	timer = 0.0;
}

GameObject* SceneTurn::FetchGO(std::string type)
{
	for (std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject *go = (GameObject *)*it;
		if (!go->GetActive() && go->GetType() == type)
		{
			go->SetActive(true);
			++m_objectCount;
			return go;
		}
	}

	if (type == "Player")
	{
		Player *go = new Player(type, true);
		StateMachineManager::GetInstance()->AddGameObject(go);
		StateMachineManager::GetInstance()->DefaultState(go);

		go->SetExitPt(MazePt(-100, -100)); // At the start, no such coordinate (for safety measures)
		go->grid.resize(m_noGrid * m_noGrid);
		go->visited.resize(m_noGrid * m_noGrid);
		std::fill(go->grid.begin(), go->grid.end(), Maze::TILE_FOG);
		std::fill(go->visited.begin(), go->visited.end(), false);
		go->stack.push_back(MazePt(go->GetPosition().x, go->GetPosition().y));
		go->grid[go->curr.y * m_noGrid + go->curr.x] = Maze::TILE_EMPTY;
		m_goList.push_back(go);
	}
	else if (type == "Enemy")
	{
		Enemy *go = new Enemy(type);
		StateMachineManager::GetInstance()->AddGameObject(go);
		StateMachineManager::GetInstance()->DefaultState(go);

		go->grid.resize(m_noGrid * m_noGrid);
		go->visited.resize(m_noGrid * m_noGrid);
		std::fill(go->grid.begin(), go->grid.end(), Maze::TILE_FOG);
		std::fill(go->visited.begin(), go->visited.end(), false);
		// Set go->curr to an empty tile
		while (true)
		{
			int posX = Math::RandIntMinMax(0, m_noGrid - 1);
			int posY = Math::RandIntMinMax(0, m_noGrid - 1);

			MazePt tile(posX, posY);
			if (SceneData::GetInstance()->m_maze.See(tile) == Maze::TILE_EMPTY)
			{
				go->curr = tile;
				break;
			}
		}
		go->stack.push_back(go->curr);
		go->grid[go->curr.y * m_noGrid + go->curr.x] = Maze::TILE_EMPTY;
		go->SeeEntireMaze();
		m_goList.push_back(go);
	}
	// other entities other than player
	else
	{
		for (unsigned i = 0; i < 2; ++i)
		{
			GameObject *go = new GameObject(type);
			m_goList.push_back(go);
		}
	}

	return FetchGO(type);
}

void SceneTurn::DFS(MazePt curr)
{
	m_visited[curr.y * m_noGrid + curr.x] = true;
	//UP
	if(curr.y < m_noGrid - 1)
	{
		MazePt next(curr.x, curr.y + 1);
		if (!m_visited[next.y * m_noGrid + next.x])
		{
			if (SceneData::GetInstance()->m_maze.Move(Maze::DIR_UP) == true)
			{
				m_myGrid[next.y * m_noGrid + next.x] = Maze::TILE_EMPTY;
				DFS(next);
				SceneData::GetInstance()->m_maze.Move(Maze::DIR_DOWN);
			}
			else
			{
				m_myGrid[next.y * m_noGrid + next.x] = Maze::TILE_WALL;
			}
		}
	}
	//DOWN
	if (curr.y > 0)
	{
		MazePt next(curr.x, curr.y - 1);
		if (!m_visited[next.y * m_noGrid + next.x])
		{
			if (SceneData::GetInstance()->m_maze.Move(Maze::DIR_DOWN) == true)
			{
				m_myGrid[next.y * m_noGrid + next.x] = Maze::TILE_EMPTY;
				DFS(next);
				SceneData::GetInstance()->m_maze.Move(Maze::DIR_UP);
			}
			else
			{
				m_myGrid[next.y * m_noGrid + next.x] = Maze::TILE_WALL;
			}
		}
	}
	//LEFT
	if (curr.x > 0)
	{
		MazePt next(curr.x - 1, curr.y);
		if (!m_visited[next.y * m_noGrid + next.x])
		{
			if (SceneData::GetInstance()->m_maze.Move(Maze::DIR_LEFT) == true)
			{
				m_myGrid[next.y * m_noGrid + next.x] = Maze::TILE_EMPTY;
				DFS(next);
				SceneData::GetInstance()->m_maze.Move(Maze::DIR_RIGHT);
			}
			else
			{
				m_myGrid[next.y * m_noGrid + next.x] = Maze::TILE_WALL;
			}
		}
	}
	//RIGHT
	if (curr.x < m_noGrid - 1)
	{
		MazePt next(curr.x + 1, curr.y);
		if (!m_visited[next.y * m_noGrid + next.x])
		{
			if (SceneData::GetInstance()->m_maze.Move(Maze::DIR_RIGHT) == true)
			{
				m_myGrid[next.y * m_noGrid + next.x] = Maze::TILE_EMPTY;
				DFS(next);
				SceneData::GetInstance()->m_maze.Move(Maze::DIR_LEFT);
			}
			else
			{
				m_myGrid[next.y * m_noGrid + next.x] = Maze::TILE_WALL;
			}
		}
	}
}

bool SceneTurn::BFS(MazePt start, MazePt end)
{
	std::fill(m_visited.begin(), m_visited.end(), false);
	while (!m_queue.empty())
		m_queue.pop();
	m_shortestPath.clear();
	m_queue.push(start);
	SceneData::GetInstance()->m_maze.SetNumMove(0);
	while (!m_queue.empty())
	{
		MazePt curr = m_queue.front();
		SceneData::GetInstance()->m_maze.SetCurr(curr);
		m_queue.pop();
		if (curr.x == end.x && curr.y == end.y)
		{
			while (!(curr.x == start.x && curr.y == start.y))
			{
				m_shortestPath.insert(m_shortestPath.begin(), curr);
				curr = m_previous[curr.y * m_noGrid + curr.x];
			}
			m_shortestPath.insert(m_shortestPath.begin(), curr);
			return true;
		}
		//UP
		if (curr.y < m_noGrid - 1)
		{
			MazePt next(curr.x, curr.y + 1);
			if (!m_visited[next.y * m_noGrid + next.x] && m_myGrid[next.y * m_noGrid + next.x] == Maze::TILE_EMPTY)
			{
				m_previous[next.y * m_noGrid + next.x] = curr;
				m_queue.push(next);
				m_visited[next.y * m_noGrid + next.x] = true;
			}
		}
		//DOWN
		if (curr.y > 0)
		{
			MazePt next(curr.x, curr.y - 1);
			if (!m_visited[next.y * m_noGrid + next.x] && m_myGrid[next.y * m_noGrid + next.x] == Maze::TILE_EMPTY)
			{
				m_previous[next.y * m_noGrid + next.x] = curr;
				m_queue.push(next);
				m_visited[next.y * m_noGrid + next.x] = true;
			}
		}
		//LEFT
		if (curr.x > 0)
		{
			MazePt next(curr.x - 1, curr.y);
			if (!m_visited[next.y * m_noGrid + next.x] && m_myGrid[next.y * m_noGrid + next.x] == Maze::TILE_EMPTY)
			{
				m_previous[next.y * m_noGrid + next.x] = curr;
				m_queue.push(next);
				m_visited[next.y * m_noGrid + next.x] = true;
			}
		}
		//RIGHT
		if (curr.x < m_noGrid - 1)
		{
			MazePt next(curr.x + 1, curr.y);
			if (!m_visited[next.y * m_noGrid + next.x] && m_myGrid[next.y * m_noGrid + next.x] == Maze::TILE_EMPTY)
			{
				m_previous[next.y * m_noGrid + next.x] = curr;
				m_queue.push(next);
				m_visited[next.y * m_noGrid + next.x] = true;
			}
		}
	}
	return false;
}

void SceneTurn::DFSOnce(GameObject *go)
{
	go->visited[go->curr.y * m_noGrid + go->curr.x] = true;
	go->stack.push_back(go->curr);
	MazePt next;

	// Check up
	next.Set(go->curr.x, go->curr.y + 1);
	if (next.y < m_noGrid && !go->visited[next.y * m_noGrid + next.x])
	{
		go->grid[next.y * m_noGrid + next.x] = SceneData::GetInstance()->m_maze.See(next);
		if (go->grid[next.y * m_noGrid + next.x] == Maze::TILE_EMPTY || go->grid[next.y * m_noGrid + next.x] == Maze::TILE_DOOR)
		{
			go->curr = next;
			return;
		}
	}

	// Check down
	next.Set(go->curr.x, go->curr.y - 1);
	if (next.y >= 0 && !go->visited[next.y * m_noGrid + next.x])
	{
		go->grid[next.y * m_noGrid + next.x] = SceneData::GetInstance()->m_maze.See(next);
		if (go->grid[next.y * m_noGrid + next.x] == Maze::TILE_EMPTY || go->grid[next.y * m_noGrid + next.x] == Maze::TILE_DOOR)
		{
			go->curr = next;
			return;
		}
	}

	// Check left
	next.Set(go->curr.x - 1, go->curr.y);
	if (next.x >= 0 && !go->visited[next.y * m_noGrid + next.x])
	{
		go->grid[next.y * m_noGrid + next.x] = SceneData::GetInstance()->m_maze.See(next);
		if (go->grid[next.y * m_noGrid + next.x] == Maze::TILE_EMPTY || go->grid[next.y * m_noGrid + next.x] == Maze::TILE_DOOR)
		{
			go->curr = next;
			return;
		}
	}

	// Check right
	next.Set(go->curr.x + 1, go->curr.y);
	if (next.x < m_noGrid && !go->visited[next.y * m_noGrid + next.x])
	{
		go->grid[next.y * m_noGrid + next.x] = SceneData::GetInstance()->m_maze.See(next);
		if (go->grid[next.y * m_noGrid + next.x] == Maze::TILE_EMPTY || go->grid[next.y * m_noGrid + next.x] == Maze::TILE_DOOR)
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

bool SceneTurn::BFSLimit(GameObject * go, MazePt end, int limit)
{
	std::vector<bool> visited(m_noGrid * m_noGrid, false);
	std::queue<MazePt> queue;
	std::vector<MazePt> previous(m_noGrid * m_noGrid);
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
				curr = previous[curr.y * m_noGrid + curr.x];
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

				if (next.x < 0 || next.x >= m_noGrid || next.y < 0 || next.y >= m_noGrid)
					continue;

				if (!visited[next.y * m_noGrid + next.x] &&
					(go->grid[next.y * m_noGrid + next.x] == Maze::TILE_EMPTY) || (go->grid[next.y * m_noGrid + next.x] == Maze::TILE_DOOR))
				{
					previous[next.y * m_noGrid + next.x] = curr;
					queue.push(next);
					visited[next.y * m_noGrid + next.x] = true;
				}
			}
		}
	}

	curr = nearestTile;

	while (curr.x != go->curr.x || curr.y != go->curr.y)
	{
		go->path.insert(go->path.begin(), curr);
		curr = previous[curr.y * m_noGrid + curr.x];
	}
	go->path.insert(go->path.begin(), curr);

	return false;
}

void SceneTurn::PlayerVisibility()
{
	for (std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject *go = (GameObject *)*it;
		if (go->GetType() != "Player")
			continue;

		if (go->curr.x < 0 || go->curr.x + 1 > m_noGrid || go->curr.y < 0 || go->curr.y + 1 > m_noGrid)
			return;

		Player *player = dynamic_cast<Player*>(go);

		fogList[go->curr.y * m_noGrid + go->curr.x] = Maze::FOG::SEEN;

		if (go->curr.x - 1 >= 0) // x position
		{
			go->grid[go->curr.y * m_noGrid + (go->curr.x - 1)] = SceneData::GetInstance()->m_maze.See(go->curr.y * m_noGrid + (go->curr.x - 1));
			fogList[go->curr.y * m_noGrid + (go->curr.x - 1)] = Maze::FOG::SEEN;

			if (go->grid[go->curr.y * m_noGrid + (go->curr.x - 1)] == Maze::TILE_DOOR)
			{
				player->SetExitPt(MazePt(go->curr.x - 1, go->curr.y));
				player->SetExitFound(true);
			}
		}
		if (go->curr.x + 1 < m_noGrid)
		{
			go->grid[go->curr.y * m_noGrid + (go->curr.x + 1)] = SceneData::GetInstance()->m_maze.See(go->curr.y * m_noGrid + (go->curr.x + 1));
			fogList[go->curr.y * m_noGrid + (go->curr.x + 1)] = Maze::FOG::SEEN;

			if (go->grid[go->curr.y * m_noGrid + (go->curr.x + 1)] == Maze::TILE_DOOR)
			{
				player->SetExitPt(MazePt(go->curr.x + 1, go->curr.y));
				player->SetExitFound(true);
			}
		}
		if (go->curr.y - 1 >= 0) // y position
		{
			go->grid[(go->curr.y - 1) * m_noGrid + go->curr.x] = SceneData::GetInstance()->m_maze.See((go->curr.y - 1) * m_noGrid + go->curr.x);
			fogList[(go->curr.y - 1) * m_noGrid + go->curr.x] = Maze::FOG::SEEN;

			if (go->grid[(go->curr.y - 1) * m_noGrid + go->curr.x] == Maze::TILE_DOOR)
			{
				player->SetExitPt(MazePt(go->curr.x, go->curr.y - 1));
				player->SetExitFound(true);
			}
		}
		if (go->curr.y + 1 < m_noGrid)
		{
			go->grid[(go->curr.y + 1) * m_noGrid + go->curr.x] = SceneData::GetInstance()->m_maze.See((go->curr.y + 1) * m_noGrid + go->curr.x);
			fogList[(go->curr.y + 1) * m_noGrid + go->curr.x] = Maze::FOG::SEEN;

			if (go->grid[(go->curr.y + 1) * m_noGrid + go->curr.x] == Maze::TILE_DOOR)
			{
				player->SetExitPt(MazePt(go->curr.x, go->curr.y + 1));
				player->SetExitFound(true);
			}
		}
	}
}

void SceneTurn::GameLogic()
{
	for (std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject *go = (GameObject *)*it;
		if (!go->GetActive())
			continue;

		if (go->GetType() != "Player")
			continue;

		Player *player = dynamic_cast<Player*>(go);
		if (player->curr.x == player->GetExitPt().x && player->curr.y == player->GetExitPt().y) // Win
		{
			gameEnded = true;
			ss2.str("");
			ss2 << "You won! Press 1 / 2 to restart the game";
		}
		else if(player->curr.x == 0 && player->curr.y == 0) // Bad maze generator problem
		{
			if (gameJustStarted)
				return;

			gameEnded = true;
			ss2.str("");
			ss2 << "Unforturnately, maze generator block you from escaping ):";
		}
		else // Lose
		{
			for (std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
			{
				GameObject *go2 = (GameObject *)*it;
				if (!go2->GetActive())
					continue;
				if (go2->GetType() != "Enemy")
					continue;

				Enemy *enemy = dynamic_cast<Enemy*>(go2);
				if (player->curr.x == go2->curr.x && player->curr.y == go2->curr.y)
				{
					gameEnded = true;
					ss2.str("");
					ss2 << "You lost.. Press 1 / 2 to restart the game";
				}
			}
		}
	}
}

void SceneTurn::Restart(int m_noGrid)
{
	m_turn = 0;
	m_mazeKey = Math::RandIntMinMax(0, 100);
	m_wallLoad = Math::RandFloatMinMax(0.25f, 0.35f);

	m_gridSize = m_worldHeight / m_noGrid;
	m_myGrid.resize(m_noGrid * m_noGrid);
	m_visited.resize(m_noGrid * m_noGrid);
	m_previous.resize(m_noGrid * m_noGrid);
	std::fill(m_myGrid.begin(), m_myGrid.end(), Maze::TILE_FOG);
	std::fill(m_visited.begin(), m_visited.end(), false);
	m_myGrid[m_start.y * m_noGrid + m_start.x] = Maze::TILE_EMPTY;
	// Fog
	fogList.resize(m_noGrid * m_noGrid);
	std::fill(fogList.begin(), fogList.end(), Maze::FOG::UNSEEN);

	SceneData::GetInstance()->m_maze.Generate(m_mazeKey, m_noGrid, m_start, m_wallLoad); //Generate new maze

	std::vector<GameObject*> tempList = m_goList;
	for (auto go : tempList)
	{
		if (!go->GetActive())
			continue;

		go->stack.clear();
		go->grid.clear();
		go->visited.clear();


		if (go->GetType() == "Player")
		{
			// Player
			go->grid.resize(m_noGrid * m_noGrid);
			go->visited.resize(m_noGrid * m_noGrid);
			std::fill(go->grid.begin(), go->grid.end(), Maze::TILE_FOG);
			std::fill(go->visited.begin(), go->visited.end(), false);
			go->stack.push_back(MazePt(go->GetPosition().x, go->GetPosition().y));
			go->grid[go->curr.y * m_noGrid + go->curr.x] = Maze::TILE_EMPTY;
			go->curr.Set(m_start.x, m_start.y);

			Player *player = dynamic_cast<Player*>(go);
			player->SetExitFound(false);
			player->SetExitPt(MazePt(-100, 100));
			StateMachineManager::GetInstance()->SetNextState(player, "Finding");
		}

		else if (go->GetType() == "Enemy")
		{
			Enemy *enemy = dynamic_cast<Enemy*>(go);

			go->grid.resize(m_noGrid * m_noGrid);
			go->visited.resize(m_noGrid * m_noGrid);
			std::fill(go->grid.begin(), go->grid.end(), Maze::TILE_FOG);
			std::fill(go->visited.begin(), go->visited.end(), false);
			// Set go->curr to an empty tile
			while (true)
			{
				int posX = Math::RandIntMinMax(0, m_noGrid - 1);
				int posY = Math::RandIntMinMax(0, m_noGrid - 1);

				MazePt tile(posX, posY);
				if (SceneData::GetInstance()->m_maze.See(tile) == Maze::TILE_EMPTY)
				{
					go->curr = tile;
					break;
				}
			}
			go->stack.push_back(go->curr);
			go->grid[go->curr.y * m_noGrid + go->curr.x] = Maze::TILE_EMPTY;
			enemy->SeeEntireMaze();
		}
	}

	tempList.clear();
}

void SceneTurn::FogAgain()
{
	std::fill(fogList.begin(), fogList.end(), Maze::FOG::UNSEEN);
}

void SceneTurn::Update(double dt)
{
	SceneBase::Update(dt);
	//FogAgain();
	PlayerVisibility();

	//Calculating aspect ratio
	m_worldHeight = 100.f;
	m_worldWidth = m_worldHeight * (float)Application::GetWindowWidth() / Application::GetWindowHeight();

	if (Application::IsKeyPressed(VK_OEM_MINUS))
	{
		m_speed = Math::Max(0.f, m_speed - 0.1f);
	}
	if (Application::IsKeyPressed(VK_OEM_PLUS))
	{
		m_speed += 0.1f;
	}
	if (Application::IsKeyPressed('N'))
	{
	}
	if (Application::IsKeyPressed('M'))
	{
	}
	if (Application::IsKeyPressed(VK_RETURN))
	{
	}
	if (Application::IsKeyPressed('R'))
	{
		//Exercise: Implement Reset button
	}

	//Input Section
	static bool bLButtonState = false;
	if (!bLButtonState && Application::IsMousePressed(0))
	{
		bLButtonState = true;
		std::cout << "LBUTTON DOWN" << std::endl;
		//double x, y;
		//Application::GetCursorPos(&x, &y);
		//int w = Application::GetWindowWidth();
		//int h = Application::GetWindowHeight();
		//float posX = static_cast<float>(x) / w * m_worldWidth;
		//float posY = (h - static_cast<float>(y)) / h * m_worldHeight;
		//if (posX < m_noGrid * m_gridSize && posY < m_noGrid * m_gridSize)
		//{
		//	m_end.Set((int)(posX / m_gridSize), (int)(posY / m_gridSize));
		//	BFS(m_start, m_end);

		//	for (auto go : m_goList)
		//		if(go->GetActive())
		//			if(go->GetType() == "Player")
		//				BFSLimit(go, m_end, 65);

		//	std::cout << "Path:";
		//	for (auto tile : m_shortestPath)
		//	{
		//		std::cout << "(" << tile.x << "," << tile.y << ")";
		//	}
		//	std::cout << std::endl;
		//}
	}
	else if (bLButtonState && !Application::IsMousePressed(0))
	{
		bLButtonState = false;
		std::cout << "LBUTTON UP" << std::endl;
	}
	static bool bRButtonState = false;
	if (!bRButtonState && Application::IsMousePressed(1))
	{
		bRButtonState = true;
		std::cout << "RBUTTON DOWN" << std::endl;
	}
	else if (bRButtonState && !Application::IsMousePressed(1))
	{
		bRButtonState = false;
		std::cout << "RBUTTON UP" << std::endl;
	}
	static bool bSpaceState = false;
	if (!bSpaceState && Application::IsKeyPressed(VK_SPACE))
	{
		bSpaceState = true;
		//GameObject *go = FetchGO("Enemy");
		if (SeeEnemy)
			SeeEnemy = false;
		else if (!SeeEnemy)
			SeeEnemy = true;
	}
	else if (bSpaceState && !Application::IsKeyPressed(VK_SPACE))
	{
		bSpaceState = false;
	}

	timer += m_speed * dt;
	SceneData::GetInstance()->SetSpeed(m_speed);
	if (timer > TURN_TIME)
	{
		++m_turn;
		timer -= TURN_TIME;
		if (gameEnded)
			return;

		StateMachineManager::GetInstance()->Update(dt);
		GameLogic();
		gameJustStarted = false;

		for (auto go : m_goList)
		{
			if (!go->GetActive())
				continue;

			if (!go->path.empty())
			{
				// Path finding state
				go->curr = go->path[0];
				go->path.erase(go->path.begin());

				// Trying to see if the player AI will avoid the enemy
				if (go->GetType() == "Enemy")
				{
					for (auto go2 : m_goList)
					{
						if (!go2->GetActive())
							continue;
						if (go2->GetType() != "Player")
							continue;

						go2->grid[go->curr.y * m_noGrid + go->curr.x] = Maze::TILE_ENEMY;
					}
				}
			}
		}
	}

	if (Application::IsKeyPressed('1') || Application::IsKeyPressed('2'))
	{

		if (Application::IsKeyPressed('1'))
		{
			m_noGrid = 10;
			SceneData::GetInstance()->SetNoGrid(m_noGrid);
		}
		else if (Application::IsKeyPressed('2'))
		{
			m_noGrid = 12;
			SceneData::GetInstance()->SetNoGrid(m_noGrid);
		}

		Restart(m_noGrid);
		SceneData::GetInstance()->SetNoGrid(m_noGrid);
		gameEnded = false;
		gameJustStarted = true;
	}
}

bool SceneTurn::Handle(Message *message)
{
	delete message;
	return false;
}

void SceneTurn::RenderGO(GameObject *go)
{
	modelStack.PushMatrix();
	modelStack.Translate(go->curr.x * m_gridSize + m_gridOffset, go->curr.y * m_gridSize + m_gridOffset, 1.f);
	modelStack.Scale(m_gridSize, m_gridSize, m_gridSize);
	if (go->GetType() == "Enemy")
	{
		if(!SeeEnemy)
			if(fogList[go->curr.y * m_noGrid + go->curr.x] == Maze::FOG::SEEN)
				RenderMesh(meshList[GEO_ENEMY], false);
		if(SeeEnemy)
			RenderMesh(meshList[GEO_ENEMY], false);
	}
	else if (go->GetType() == "Player")
		RenderMesh(meshList[GEO_PLAYER], false);
	modelStack.PopMatrix();
}

void SceneTurn::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Projection matrix : Orthographic Projection
	Mtx44 projection;
	projection.SetToOrtho(0, m_worldWidth, 0, m_worldHeight, -10, 10);
	projectionStack.LoadMatrix(projection);

	// Camera matrix
	viewStack.LoadIdentity();
	viewStack.LookAt(
		camera.position.x, camera.position.y, camera.position.z,
		camera.target.x, camera.target.y, camera.target.z,
		camera.up.x, camera.up.y, camera.up.z
		);
	// Model matrix : an identity matrix (model will be at the origin)
	modelStack.LoadIdentity();

	RenderMesh(meshList[GEO_AXES], false);

	modelStack.PushMatrix();
	modelStack.Translate(m_worldHeight * 0.5f, m_worldHeight * 0.5f, -1.f);
	modelStack.Scale(m_worldHeight, m_worldHeight, m_worldHeight);
	RenderMesh(meshList[GEO_WHITEQUAD], false);
	modelStack.PopMatrix();

	//Render tiles 
	if (fog)
	{
		for (int row = 0; row < m_noGrid; ++row)
		{
			for (int col = 0; col < m_noGrid; ++col)
			{
				modelStack.PushMatrix();
				modelStack.Translate(m_gridOffset + m_gridSize * col, m_gridOffset + m_gridSize * row, 0);
				modelStack.Scale(m_gridSize, m_gridSize, m_gridSize);

				if (fogList[row * m_noGrid + col] == Maze::FOG::SEEN)
				{
					switch (SceneData::GetInstance()->m_maze.m_grid[row * m_noGrid + col])
					{
					case Maze::TILE_WALL:
						RenderMesh(meshList[GEO_GREYQUAD], false);
						break;
					case Maze::TILE_EMPTY:
						RenderMesh(meshList[GEO_WHITEQUAD], false);
						break;
					case Maze::TILE_DOOR:
						RenderMesh(meshList[GEO_DOOR], false);
						break;
					}
				}
				else
					RenderMesh(meshList[GEO_BLACKQUAD], false);
				modelStack.PopMatrix();
			}
		}
	}

	//Render curr point
	//MazePt curr = m_maze.GetCurr();
	//modelStack.PushMatrix();
	//modelStack.Translate(m_gridOffset + m_gridSize * curr.x, m_gridOffset + m_gridSize * curr.y, 1);
	//modelStack.Scale(m_gridSize, m_gridSize, m_gridSize);
	//RenderMesh(meshList[GEO_QUEEN], false);
	//modelStack.PopMatrix();
	//Render shortest path
	//for (auto tile : m_shortestPath)
	//{
	//	modelStack.PushMatrix();
	//	modelStack.Translate(m_gridOffset + m_gridSize * tile.x, m_gridOffset + m_gridSize * tile.y, 1);
	//	modelStack.Scale(m_gridSize, m_gridSize, m_gridSize);
	//	RenderMesh(meshList[GEO_QUEEN], false);
	//	modelStack.PopMatrix();
	//}

	//modelStack.PushMatrix();
	//modelStack.Translate(m_gridOffset + m_gridSize * player->GetPosition().x, m_gridOffset + m_gridSize * player->GetPosition().y, 1.f);
	//modelStack.Scale(m_gridSize, m_gridSize, m_gridSize);
	//RenderMesh(meshList[GEO_PLAYER], false);
	//modelStack.PopMatrix();

	//Render m_goList
	for (std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject *go = (GameObject *)*it;
		if (go->GetActive())
			RenderGO(go);
	}

	//On screen text
	std::ostringstream ss;
	ss.precision(3);
	ss << "Speed:" << m_speed;
	RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 50, 6);

	ss.str("");
	ss.precision(5);
	ss << "FPS:" << fps;
	RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 50, 3);

	ss.str("");
	ss << "Turn:" << m_turn;
	RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 50, 9);

	if (gameEnded)
		RenderTextOnScreen(meshList[GEO_TEXT], ss2.str(), Color(0, 1, 0), 3, 10, 50);

	//ss.str("");
	//ss << "Turn Maze " << m_mazeKey;
	//RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 50, 0);
}

void SceneTurn::Exit()
{
	SceneBase::Exit();
	//Cleanup GameObjects
	while (m_goList.size() > 0)
	{
		GameObject *go = m_goList.back();
		delete go;
		m_goList.pop_back();
	}
}
