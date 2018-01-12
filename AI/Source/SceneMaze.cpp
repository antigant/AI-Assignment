#include "SceneMaze.h"
#include "GL\glew.h"
#include "Application.h"
#include <sstream>

SceneMaze::SceneMaze()
{
}

SceneMaze::~SceneMaze()
{
}

void SceneMaze::Init()
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

	m_start.Set(0, 0);
	m_mazeKey = 0;
	m_maze.Generate(m_mazeKey, m_noGrid, m_start, 0.3f);
	m_myGrid.resize(m_noGrid * m_noGrid);
	m_visited.resize(m_noGrid * m_noGrid);
	m_previous.resize(m_noGrid * m_noGrid);
	std::fill(m_myGrid.begin(), m_myGrid.end(), Maze::TILE_FOG);
	std::fill(m_visited.begin(), m_visited.end(), false);
	m_myGrid[m_start.y * m_noGrid + m_start.x] = Maze::TILE_EMPTY;
	DFS(m_start);
}

GameObject* SceneMaze::FetchGO(std::string type)
{
	for (std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject *go = (GameObject *)*it;
		if (!go->GetActive())
		{
			go->SetActive(true);
			++m_objectCount;
			return go;
		}
	}
	for (unsigned i = 0; i < 10; ++i)
	{
		GameObject *go = new GameObject(type);
		m_goList.push_back(go);
	}
	return FetchGO(type);
}

void SceneMaze::DFS(MazePt curr)
{
	m_visited[curr.y * m_noGrid + curr.x] = true;
	MazePt next;
	next.Set(curr.x, curr.y + 1);
	if (next.y < m_noGrid && !m_visited[next.y * m_noGrid + next.x])
	{
		if (m_maze.Move(Maze::DIR_UP))
		{
			m_myGrid[next.y * m_noGrid + next.x] = Maze::TILE_EMPTY;
			DFS(next);
			m_maze.Move(Maze::DIR_DOWN);
		}
		else
			m_myGrid[next.y * m_noGrid + next.x] = Maze::TILE_WALL;
	}
	next.Set(curr.x, curr.y - 1);
	if (next.y >= 0 && !m_visited[next.y * m_noGrid + next.x])
	{
		if (m_maze.Move(Maze::DIR_DOWN))
		{
			m_myGrid[next.y * m_noGrid + next.x] = Maze::TILE_EMPTY;
			DFS(next);
			m_maze.Move(Maze::DIR_UP);
		}
		else
			m_myGrid[next.y * m_noGrid + next.x] = Maze::TILE_WALL;
	}
	next.Set(curr.x - 1, curr.y);
	if (next.x >= 0 && !m_visited[next.y * m_noGrid + next.x])
	{
		if (m_maze.Move(Maze::DIR_LEFT))
		{
			m_myGrid[next.y * m_noGrid + next.x] = Maze::TILE_EMPTY;
			DFS(next);
			m_maze.Move(Maze::DIR_RIGHT);
		}
		else
			m_myGrid[next.y * m_noGrid + next.x] = Maze::TILE_WALL;
	}
	next.Set(curr.x + 1, curr.y);
	if (next.x < m_noGrid && !m_visited[next.y * m_noGrid + next.x])
	{
		if (m_maze.Move(Maze::DIR_RIGHT))
		{
			m_myGrid[next.y * m_noGrid + next.x] = Maze::TILE_EMPTY;
			DFS(next);
			m_maze.Move(Maze::DIR_LEFT);
		}
		else
			m_myGrid[next.y * m_noGrid + next.x] = Maze::TILE_WALL;
	}
}

void SceneMaze::Update(double dt)
{
	SceneBase::Update(dt);

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
		double x, y;
		Application::GetCursorPos(&x, &y);
		int w = Application::GetWindowWidth();
		int h = Application::GetWindowHeight();
		float posX = static_cast<float>(x) / w * m_worldWidth;
		float posY = (h - static_cast<float>(y)) / h * m_worldHeight;
		
		//Exercise: turn mouse click into end point and run BFS
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
	}
	else if (bSpaceState && !Application::IsKeyPressed(VK_SPACE))
	{
		bSpaceState = false;
	}
}


void SceneMaze::RenderGO(GameObject *go)
{

}

void SceneMaze::Render()
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

	//modelStack.PushMatrix();
	//modelStack.Translate(m_worldHeight * 0.5f, m_worldHeight * 0.5f, -1.f);
	//modelStack.Scale(m_worldHeight, m_worldHeight, m_worldHeight);
	//RenderMesh(meshList[GEO_WHITEQUAD], false);
	//modelStack.PopMatrix();

	//Render tiles 
	for (int i = 0; i < m_myGrid.size(); ++i)
	{
		int gridX = i % m_noGrid;
		int gridY = i / m_noGrid;

		modelStack.PushMatrix();
		modelStack.Translate(m_gridOffset + gridX * m_gridSize, m_gridOffset + gridY * m_gridSize, 0.f);
		modelStack.Scale(m_gridSize, m_gridSize, m_gridSize);
		switch (m_myGrid[i])
		{
		case Maze::TILE_EMPTY:
			RenderMesh(meshList[GEO_WHITEQUAD], false);
			break;
		case Maze::TILE_FOG:
			RenderMesh(meshList[GEO_BLACKQUAD], false);
			break;
		case Maze::TILE_WALL:
			RenderMesh(meshList[GEO_GREYQUAD], false);
			break;
		}
		modelStack.PopMatrix();
	}
	//Render curr point
	modelStack.PushMatrix();
	modelStack.Translate(m_gridOffset + m_maze.GetCurr().x * m_gridSize, m_gridOffset + m_maze.GetCurr().y * m_gridSize, 0.2f);
	modelStack.Scale(m_gridSize, m_gridSize, m_gridSize);
	RenderMesh(meshList[GEO_QUEEN], false);
	modelStack.PopMatrix();
	//Render shortest path
	for (int i = 0; i < m_shortestPath.size(); ++i)
	{
		modelStack.PushMatrix();
		modelStack.Translate(m_gridOffset + m_maze.GetCurr().x * m_gridSize, m_gridOffset + m_maze.GetCurr().y * m_gridSize, 0.2f);
		modelStack.Scale(m_gridSize, m_gridSize, m_gridSize);
		RenderMesh(meshList[GEO_QUEEN], false);
		modelStack.PopMatrix();
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
	ss << "Num Move:" << m_maze.GetNumMove();
	RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 50, 9);

	ss.str("");
	ss << "Maze " << m_mazeKey;
	RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 50, 0);
}

void SceneMaze::Exit()
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
