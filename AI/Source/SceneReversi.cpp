#include "SceneReversi.h"
#include "GL\glew.h"
#include "Application.h"
#include <sstream>

#include "SceneData.h"

SceneReversi::SceneReversi()
{
}

SceneReversi::~SceneReversi()
{
}

void SceneReversi::Init()
{
	SceneBase::Init();

	//Calculating aspect ratio
	m_worldHeight = 100.f;
	m_worldWidth = m_worldHeight * (float)Application::GetWindowWidth() / Application::GetWindowHeight();

	//Physics code here
	m_speed = 1.f;

	Math::InitRNG();

	SceneData::GetInstance()->SetNoGrid(8);
	m_grid = new WHO_CONTENT[SceneData::GetInstance()->GetNoGrid() * SceneData::GetInstance()->GetNoGrid()];
	SceneData::GetInstance()->SetGridSize(m_worldHeight / SceneData::GetInstance()->GetNoGrid());
	SceneData::GetInstance()->SetGridOffset(SceneData::GetInstance()->GetGridSize() * 0.5f);

	Reset();
}

GameObject* SceneReversi::FetchGO(const std::string type)
{
	for (std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject *go = (GameObject *)*it;
		if (!go->GetActive())
		{
			go->SetActive(true);
			SceneData::GetInstance()->AddObjectCount(1);
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

//Exercise: This function should return 0 to N, i.e. AI player's decision to put its disk on one of the grids
int SceneReversi::GetAIDecision()
{
	return 0;
}

void SceneReversi::Reset()
{
	SceneData::GetInstance()->SetObjectCount(0);
	m_bBlackTurn = true;
	m_bGameOver = false;
	m_winner = WHO_CONTENT::WHO_NONE;
	for (int i = 0; i < SceneData::GetInstance()->GetNoGrid() * SceneData::GetInstance()->GetNoGrid(); ++i)
	{
		m_grid[i] = WHO_NONE;
	}
	int half = SceneData::GetInstance()->GetNoGrid() * 0.5f - 1;
	m_grid[half * SceneData::GetInstance()->GetNoGrid() + half] = WHO_BLACK;
	m_grid[half * SceneData::GetInstance()->GetNoGrid() + half + 1] = WHO_WHITE;
	m_grid[(half + 1) * SceneData::GetInstance()->GetNoGrid() + half] = WHO_WHITE;
	m_grid[(half + 1) * SceneData::GetInstance()->GetNoGrid() + half + 1] = WHO_BLACK;

}

bool SceneReversi::IsInBoard(int posX, int posY)
{
	if (posX < 0)
		return false;

	if (posY < 0)
		return false;

	if (posX >= SceneData::GetInstance()->GetNoGrid())
		return false;

	if (posY >= SceneData::GetInstance()->GetNoGrid())
		return false;

	return false;
}

int SceneReversi::ConvertXYPos(int posX, int posY)
{
	return posY * SceneData::GetInstance()->GetNoGrid() + posX;
}

void SceneReversi::ConvertArrayPos(int pos, int &posX, int &posY)
{
	posX = pos % SceneData::GetInstance()->GetNoGrid();
	posY = pos / SceneData::GetInstance()->GetNoGrid();
}

// Magic function to call recursive function
// Possible change / new function : Make it count the total eaten tiles
bool SceneReversi::IsLegalMove(int posX, int posY, bool flip)
{
	int result = 0;

	if (CheckSlot(posX + 1, posY, 1, 0, 0, flip))
		++result;
	if (CheckSlot(posX - 1, posY, -1, 0, 0, flip))
		++result;
	if (CheckSlot(posX, posY + 1, 0, 1, 0, flip))
		++result;
	if (CheckSlot(posX, posY - 1, 0, -1, 0, flip))
		++result;
	if (CheckSlot(posX + 1, posY + 1, 1, 1, 0, flip))
		++result;
	if (CheckSlot(posX + 1, posY - 1, 1, -1, 0, flip))
		++result;
	if (CheckSlot(posX - 1, posY + 1, -1, 1, 0, flip))
		++result;
	if (CheckSlot(posX - 1, posY - 1, -1, -1, 0, flip))
		++result;

	return result > 0;
}

// This function is maed to recuse in *ONE DIRECTION*
// Checks for any legal matching pair across that direction

// Possible change : 
// Maybe a boolean to check to flip?
bool SceneReversi::CheckSlot(int posX, int posY, int dirX, int dirY, int count, bool flip)
{
	// Check if recursive can move in a dir
	// Terminating clauses to end the recursion
	if (!IsInBoard(posX, posY))
	{
		// Not in board, confirm fail
		return false;
	}

	if (m_grid[ConvertXYPos(posX, posY)] == WHO_NONE)
	{
		// Hit empty space, failed too
		return false;
	}

	WHO_CONTENT searchValue;
	if (m_bBlackTurn)
		searchValue = WHO_BLACK;
	else
		searchValue = WHO_WHITE;

	bool result = false;
	// Searching for same color
	if (m_grid[ConvertXYPos(posX, posY)] == searchValue)
	{
		// Same color!!! Found our target!
		if (count == 0) // This case is for adjacent same color, not acceptable legal move
			return false;

		return true;
	}
	else
	{
		// This part we have to recurse
		result = CheckSlot(posX + dirX, posY + dirY, dirX, dirY, count + 1);

		if (result && flip) // && flip == true
		{
			// Flip my current position to the color
			m_grid[ConvertXYPos(posX, posY)] = searchValue;
		}
	}

	return result;
}

void SceneReversi::Update(double dt)
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
		Reset();
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
		if (posX > 0 && posX < SceneData::GetInstance()->GetNoGrid() * SceneData::GetInstance()->GetGridSize() && posY > 0 && posY < SceneData::GetInstance()->GetNoGrid() * SceneData::GetInstance()->GetGridSize())
		{
			int gridX = static_cast<int>(posX / SceneData::GetInstance()->GetGridSize());
			int gridY = static_cast<int>(posY / SceneData::GetInstance()->GetGridSize());
			int index = gridY * SceneData::GetInstance()->GetNoGrid() + gridX;
			if (m_grid[index] == WHO_CONTENT::WHO_NONE && IsLegalMove(gridX, gridY))
			{
				// Make the move
				IsLegalMove(gridX, gridY, true);
				if (m_bBlackTurn)
				{
					//Exercise: Check if this input is a valid move
					m_grid[index] = WHO_CONTENT::WHO_BLACK;
					m_bBlackTurn = false;
				}
				else //Exercise: For AI to take over
				{
					//Exercise: Check if this input is a valid move
					m_grid[index] = WHO_CONTENT::WHO_WHITE;
					m_bBlackTurn = true;
				}

				//Game Logic Section - call methods
				//Exercise: Flips the existing disks on board
				//Exercise: Check if there is valid move for current color, if not forfeit the turn 
				//Exercise: Check draw, black wins or white wins
			}
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
		for (int i = 0; i < SceneData::GetInstance()->GetNoGrid(); ++i)
		{
			// Testing AI stuff (prints out all legal moves for me)
			int x, y;
			ConvertArrayPos(i, x, y);
			if (IsLegalMove(x, y))
			{
				std::cout << "LEGAL MOVE : " << x << " " << y << std::endl;
			}
		}
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


void SceneReversi::RenderGO(GameObject *go)
{
}

void SceneReversi::Render()
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
	if(SceneData::GetInstance()->GetNoGrid() == 4)
		RenderMesh(meshList[GEO_REVERSI4X4], false);
	else if(SceneData::GetInstance()->GetNoGrid() == 8)
		RenderMesh(meshList[GEO_REVERSI], false);
	modelStack.PopMatrix();

	for (int i = 0; i < SceneData::GetInstance()->GetNoGrid() * SceneData::GetInstance()->GetNoGrid(); ++i)
	{
		if (m_grid[i] == WHO_BLACK)
		{
			int gridX = i % SceneData::GetInstance()->GetNoGrid();
			int gridY = i / SceneData::GetInstance()->GetNoGrid();
			modelStack.PushMatrix();
			modelStack.Translate(SceneData::GetInstance()->GetGridOffSet() + gridX * SceneData::GetInstance()->GetGridSize(), SceneData::GetInstance()->GetGridOffSet() + gridY * SceneData::GetInstance()->GetGridSize(), 0);
			modelStack.Scale(SceneData::GetInstance()->GetGridSize(), SceneData::GetInstance()->GetGridSize(), 1);
			RenderMesh(meshList[GEO_BLACK], false);
			modelStack.PopMatrix();
		}
		else if (m_grid[i] == WHO_WHITE)
		{
			int gridX = i % SceneData::GetInstance()->GetNoGrid();
			int gridY = i / SceneData::GetInstance()->GetNoGrid();
			modelStack.PushMatrix();
			modelStack.Translate(SceneData::GetInstance()->GetGridOffSet() + gridX * SceneData::GetInstance()->GetGridSize(), SceneData::GetInstance()->GetGridOffSet() + gridY * SceneData::GetInstance()->GetGridSize(), 0);
			modelStack.Scale(SceneData::GetInstance()->GetGridSize(), SceneData::GetInstance()->GetGridSize(), 1);
			RenderMesh(meshList[GEO_WHITE], false);
			modelStack.PopMatrix();
		}
	}

	for (std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject *go = (GameObject *)*it;
		if (go->GetActive())
			RenderGO(go);
	}

	//On screen text
	static std::string winnerText[] = { "Draw", "Black wins", "White wins" };

	std::ostringstream ss;
	ss.precision(3);
	ss << "Speed:" << m_speed;
	RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 50, 6);

	ss.str("");
	ss.precision(5);
	ss << "FPS:" << fps;
	RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 50, 3);

	ss.str("");
	ss << "Count:" << SceneData::GetInstance()->GetObjectCount();
	RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 50, 9);

	ss.str("");
	ss << "Turn:" << (m_bBlackTurn ? "Black" : "White");
	RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 50, 12);

	if (m_bGameOver)
	{
		ss.str("");
		ss << winnerText[static_cast<int>(m_winner)];
		RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 50, 15);
	}

	RenderTextOnScreen(meshList[GEO_TEXT], "Reversi (R to reset)", Color(0, 1, 0), 3, 50, 0);
}

void SceneReversi::Exit()
{
	SceneBase::Exit();
	//Cleanup GameObjects
	while (m_goList.size() > 0)
	{
		GameObject *go = m_goList.back();
		delete go;
		m_goList.pop_back();
	}
	if (m_ghost)
	{
		delete m_ghost;
		m_ghost = NULL;
	}
}
