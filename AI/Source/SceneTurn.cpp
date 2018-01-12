#include "SceneTurn.h"
#include "GL\glew.h"
#include "Application.h"
#include <sstream>

#define TURN_TIME 0.5

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
	m_gridSize = m_worldHeight / m_noGrid;
	m_gridOffset = m_gridSize / 2;

	// Enable fog
	fog = true;

	fogList.resize(m_noGrid * m_noGrid);
	std::fill(fogList.begin(), fogList.end(), Maze::FOG::UNSEEN);

	// Player
	player = FetchGO("Player");
	//player->SetPosition(Vector3(5.f, 5.f, 0.f));
	//fogList[player->GetPosition().y * m_noGrid + player->GetPosition().x] = Maze::FOG::SEEN;
	//fogList[(player->GetPosition().y + 1) * m_noGrid + player->GetPosition().x] = Maze::FOG::SEEN;
	//fogList[(player->GetPosition().y - 1) * m_noGrid + player->GetPosition().x] = Maze::FOG::SEEN;
	//fogList[player->GetPosition().y * m_noGrid + (player->GetPosition().x + 1)] = Maze::FOG::SEEN;

	m_start.Set(0, 0);
	m_mazeKey = 0;
	m_wallLoad = 0.3f;
	m_maze.Generate(m_mazeKey, m_noGrid, m_start, m_wallLoad); //Generate new maze
	m_myGrid.resize(m_noGrid * m_noGrid);
	m_visited.resize(m_noGrid * m_noGrid);
	m_previous.resize(m_noGrid * m_noGrid);
	std::fill(m_myGrid.begin(), m_myGrid.end(), Maze::TILE_FOG);
	std::fill(m_visited.begin(), m_visited.end(), false);
	m_myGrid[m_start.y * m_noGrid + m_start.x] = Maze::TILE_EMPTY;
	DFS(m_start);

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
	for (unsigned i = 0; i < 2; ++i)
	{
		GameObject *go = new GameObject(type);
		m_goList.push_back(go);
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
			if (m_maze.Move(Maze::DIR_UP) == true)
			{
				m_myGrid[next.y * m_noGrid + next.x] = Maze::TILE_EMPTY;
				DFS(next);
				m_maze.Move(Maze::DIR_DOWN);
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
			if (m_maze.Move(Maze::DIR_DOWN) == true)
			{
				m_myGrid[next.y * m_noGrid + next.x] = Maze::TILE_EMPTY;
				DFS(next);
				m_maze.Move(Maze::DIR_UP);
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
			if (m_maze.Move(Maze::DIR_LEFT) == true)
			{
				m_myGrid[next.y * m_noGrid + next.x] = Maze::TILE_EMPTY;
				DFS(next);
				m_maze.Move(Maze::DIR_RIGHT);
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
			if (m_maze.Move(Maze::DIR_RIGHT) == true)
			{
				m_myGrid[next.y * m_noGrid + next.x] = Maze::TILE_EMPTY;
				DFS(next);
				m_maze.Move(Maze::DIR_LEFT);
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
	m_maze.SetNumMove(0);
	while (!m_queue.empty())
	{
		MazePt curr = m_queue.front();
		m_maze.SetCurr(curr);
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
		go->grid[next.y * m_noGrid + next.x] = m_maze.See(next);
		if (go->grid[next.y * m_noGrid + next.x] == Maze::TILE_EMPTY)
		{
			go->curr = next;
			return;
		}
	}

	// Check down
	next.Set(go->curr.x, go->curr.y - 1);
	if (next.y >= 0 && !go->visited[next.y * m_noGrid + next.x])
	{
		go->grid[next.y * m_noGrid + next.x] = m_maze.See(next);
		if (go->grid[next.y * m_noGrid + next.x] == Maze::TILE_EMPTY)
		{
			go->curr = next;
			return;
		}
	}

	// Check left
	next.Set(go->curr.x - 1, go->curr.y);
	if (next.x >= 0 && !go->visited[next.y * m_noGrid + next.x])
	{
		go->grid[next.y * m_noGrid + next.x] = m_maze.See(next);
		if (go->grid[next.y * m_noGrid + next.x] == Maze::TILE_EMPTY)
		{
			go->curr = next;
			return;
		}
	}

	// Check right
	next.Set(go->curr.x + 1, go->curr.y);
	if (next.x < m_noGrid && !go->visited[next.y * m_noGrid + next.x])
	{
		go->grid[next.y * m_noGrid + next.x] = m_maze.See(next);
		if (go->grid[next.y * m_noGrid + next.x] == Maze::TILE_EMPTY)
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

//bool SceneTurn::BFSLimit(GameObject *go, MazePt end, int limit)
//{
//	std::vector<bool> visited(m_noGrid * m_noGrid, false);
//	std::queue<MazePt> queue;
//	std::vector<MazePt> previous(m_noGrid * m_noGrid); // to store previous tile
//	queue.push(go->curr);
//	int nearestDistance = INT_MAX;
//	MazePt nearestTile = go->curr;
//	MazePt curr;
//	int loop = 0;
//	while (!queue.empty() && loop < limit)
//	{
//		++loop;
//		curr = queue.front();
//		queue.pop();
//		visited[curr.y * m_noGrid + curr.y] = true;
//
//		int distance = abs(curr.x - end.x) + abs(curr.y - end.y);
//		if (distance < nearestDistance)
//		{
//			nearestDistance = distance;
//			nearestTile = curr;
//		}
//
//		if (curr.x == end.x && curr.y == end.y)
//		{
//			// Shortest path to end tile
//			while (curr.x != go->curr.x || curr.y != go->curr.y) // condition might be ||
//			{
//				go->path.insert(go->path.begin(), curr);
//				curr = previous[curr.y * m_noGrid + curr.x];
//			}
//			go->path.insert(go->path.begin(), curr);
//			return true;
//		}
//
//		// Check Up Direction
//		for (int y = 1; y >= -1; --y)
//		{
//			for (int x = 1; x >= -1; --x)
//			{
//				if (abs(x - y) != 1)
//					continue;
//
//				MazePt next(curr.x + x, curr.y + y);
//				if (next.x < 0 || next.x >= m_noGrid || next.y < 0 || next.y >= m_noGrid)
//					continue;
//
//				if (!visited[next.y * m_noGrid + next.x] && m_maze.m_grid[next.y * m_noGrid + next.x] == Maze::TILE_EMPTY)
//				{
//					previous[next.y * m_noGrid + next.x] = curr;
//					queue.push(next);
//					//visited[next.y * m_noGrid + next.y] = true;
//				}
//
//			}
//		}
//	}
//
//	//curr = nearestTile;
//	//
//	//while (curr.x != go->curr.x || curr.y != go->curr.y)
//	//{
//	//	go->path.insert(go->path.begin(), curr);
//	//	curr = previous[curr.y * m_noGrid + curr.x];
//	//}
//	//go->path.insert(go->path.begin(), curr);
//
//	return false;
//}

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
					(go->grid[next.y * m_noGrid + next.x] == Maze::TILE_EMPTY))
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
	// Standing position
	if (player->GetPosition().x < 0 || player->GetPosition().x + 1 > m_noGrid || player->GetPosition().y < 0 || player->GetPosition().y + 1 > m_noGrid)
		return;

	fogList[player->GetPosition().y * m_noGrid + player->GetPosition().x] = Maze::FOG::SEEN;

	if (player->GetPosition().x - 1 >= 0) // x position
		fogList[player->GetPosition().y * m_noGrid + (player->GetPosition().x - 1)] = Maze::FOG::SEEN;
	if (player->GetPosition().x + 1 < m_noGrid)
		fogList[player->GetPosition().y * m_noGrid + (player->GetPosition().x + 1)] = Maze::FOG::SEEN;
	if (player->GetPosition().y - 1 >= 0) // y position
		fogList[(player->GetPosition().y - 1) * m_noGrid + player->GetPosition().x] = Maze::FOG::SEEN;
	if (player->GetPosition().y + 1 < m_noGrid)
		fogList[(player->GetPosition().y + 1) * m_noGrid + player->GetPosition().x] = Maze::FOG::SEEN;

	//if (player->GetPosition().x - 1 > 0) // x position
	//{
	//	fogList[player->GetPosition().y * m_noGrid + (player->GetPosition().x - 1)] = Maze::FOG::SEEN;
	//	if(player->GetPosition().x + 1 < m_noGrid)
	//		fogList[player->GetPosition().y * m_noGrid + (player->GetPosition().x + 1)] = Maze::FOG::UNSEEN;
	//}
	//if (player->GetPosition().x + 1 < m_noGrid)
	//{
	//	fogList[player->GetPosition().y * m_noGrid + (player->GetPosition().x + 1)] = Maze::FOG::SEEN;
	//	if(player->GetPosition().x - 1 > 0)
	//		fogList[player->GetPosition().y * m_noGrid + (player->GetPosition().x - 1)] = Maze::FOG::UNSEEN;
	//}
	//if (player->GetPosition().y - 1 > 0) // y position
	//{
	//	fogList[(player->GetPosition().y - 1) * m_noGrid + player->GetPosition().x] = Maze::FOG::SEEN;
	//	if (player->GetPosition().y + 1 < m_noGrid)
	//		fogList[(player->GetPosition().y + 1) * m_noGrid + player->GetPosition().x] = Maze::FOG::UNSEEN;
	//}
	//if (player->GetPosition().y + 1 < m_noGrid)
	//{
	//	fogList[(player->GetPosition().y + 1) * m_noGrid + player->GetPosition().x] = Maze::FOG::SEEN;
	//	if(player->GetPosition().y - 1 > 0)
	//		fogList[(player->GetPosition().y - 1) * m_noGrid + player->GetPosition().x] = Maze::FOG::UNSEEN;
	//}
}

void SceneTurn::Update(double dt)
{
	SceneBase::Update(dt);
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

	if (Application::IsKeyPressed('W'))
		player->SetPosition(Vector3(player->GetPosition().x, player->GetPosition().y + 1, player->GetPosition().z));
	if (Application::IsKeyPressed('S'))
		player->SetPosition(Vector3(player->GetPosition().x, player->GetPosition().y - 1, player->GetPosition().z));
	if (Application::IsKeyPressed('A'))
		player->SetPosition(Vector3(player->GetPosition().x - 1, player->GetPosition().y, player->GetPosition().z));
	if (Application::IsKeyPressed('D'))
		player->SetPosition(Vector3(player->GetPosition().x + 1, player->GetPosition().y, player->GetPosition().z));

	//Input Section
	static bool bLButtonState = false;
	if (!bLButtonState && Application::IsMousePressed(0))
	{
		bLButtonState = true;
		std::cout << "LBUTTON DOWN" << std::endl;
		double x, y;
		Application::GetCursorPos(&x, &y);
		int w = Application::GetWindowWidth();
		int h = Application::GetWindowHeight();
		float posX = static_cast<float>(x) / w * m_worldWidth;
		float posY = (h - static_cast<float>(y)) / h * m_worldHeight;
		if (posX < m_noGrid * m_gridSize && posY < m_noGrid * m_gridSize)
		{
			m_end.Set((int)(posX / m_gridSize), (int)(posY / m_gridSize));
			BFS(m_start, m_end);

			for (auto go : m_goList)
				if(go->GetActive())
					BFSLimit(go, m_end, 40);

			std::cout << "Path:";
			for (auto tile : m_shortestPath)
			{
				std::cout << "(" << tile.x << "," << tile.y << ")";
			}
			std::cout << std::endl;
		}
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
		GameObject *go = FetchGO("NPC");
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
			if (m_maze.See(tile) == Maze::TILE_EMPTY)
			{
				go->curr = tile;
				break;
			}
		}
		go->stack.push_back(go->curr);
		go->grid[go->curr.y * m_noGrid + go->curr.x] = Maze::TILE_EMPTY;
	}
	else if (bSpaceState && !Application::IsKeyPressed(VK_SPACE))
	{
		bSpaceState = false;
	}

	timer += m_speed * dt;
	if (timer > TURN_TIME)
	{
		++m_turn;
		timer -= TURN_TIME;
		for (auto go : m_goList)
		{
			//if (go->GetActive())
			//{
			//	DFSOnce(go);

			//	if (!go->path.empty())
			//	{
			//		go->curr = go->path.front();
			//		go->path.erase(go->path.begin());
			//	}
			//}

			if (!go->GetActive())
				continue;
			if(go->GetType() == "NPC")
			{
				if (!go->stack.empty())
				{
					// Maze reading state
					DFSOnce(go);
				}
				else if (!go->path.empty())
				{
					// Path finding state
					go->curr = go->path[0];
					go->path.erase(go->path.begin());
				}
			}
		}
	}
}


void SceneTurn::RenderGO(GameObject *go)
{
	modelStack.PushMatrix();
	modelStack.Translate(go->curr.x * m_gridSize + m_gridOffset, go->curr.y * m_gridSize + m_gridOffset, 1.f);
	modelStack.Scale(m_gridSize, m_gridSize, m_gridSize);
	if (go->GetType() == "NPC")
		RenderMesh(meshList[GEO_ENEMY], false);
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
					switch (m_maze.m_grid[row * m_noGrid + col])
					{
					case Maze::TILE_WALL:
						RenderMesh(meshList[GEO_GREYQUAD], false);
						break;
					case Maze::TILE_EMPTY:
						RenderMesh(meshList[GEO_WHITEQUAD], false);
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

	modelStack.PushMatrix();
	modelStack.Translate(m_gridOffset + m_gridSize * player->GetPosition().x, m_gridOffset + m_gridSize * player->GetPosition().y, 1.f);
	modelStack.Scale(m_gridSize, m_gridSize, m_gridSize);
	RenderMesh(meshList[GEO_PLAYER], false);
	modelStack.PopMatrix();

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

	ss.str("");
	ss << "Turn Maze " << m_mazeKey;
	RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 50, 0);
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
