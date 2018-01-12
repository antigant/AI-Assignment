#include "SceneMovement.h"
#include "GL\glew.h"
#include "Application.h"
#include <sstream>

// SceneData
#include "SceneData.h"
// Message
#include "PostOffice.h"
#include "ConcreteMessages.h"

#include "StateMachineManager.h"

// All states
//#include "StatesFish.h"
//#include "StateDead.h"
//#include "StateHungry.h"
//#include "StateFull.h"

SceneMovement::SceneMovement()
{
}

SceneMovement::~SceneMovement()
{
}

void SceneMovement::Init()
{
	SceneBase::Init();
	SceneData::GetInstance()->Init();

	//Calculating aspect ratio
	m_worldHeight = 100.f;
	m_worldWidth = m_worldHeight * (float)Application::GetWindowWidth() / Application::GetWindowHeight();

	//Physics code here
	SceneData::GetInstance()->SetSpeed(1.f);
	
	Math::InitRNG();

	SceneData::GetInstance()->SetObjectCount(0);
	SceneData::GetInstance()->SetNoGrid(20);
	SceneData::GetInstance()->SetGridSize(m_worldHeight / SceneData::GetInstance()->GetNoGrid());
	SceneData::GetInstance()->SetGridOffset(SceneData::GetInstance()->GetGridSize() / 2);

	m_hourOfTheDay = 0;

	PostOffice::GetInstance()->Register("Scene", this); 
}

GameObject* SceneMovement::FetchGO(std::string type)
{
	for (std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject *go = (GameObject *)*it;
		if (!go->GetActive() && type == go->GetType())
		{
			go->SetActive(true);
			StateMachineManager::GetInstance()->DefaultState(go);
			SceneData::GetInstance()->AddObjectCount(1);
			return go;
		}
	}

	//GameObject *go = new GameObject(type);
	//if (go->type == GameObject::GO_FISH)
	//{
	//	go->sm = new StateMachine();
	//	go->sm->AddState(new StateTooFull("TOO FULL", go));
	//	go->sm->AddState(new StateFull("FULL", go));
	//	go->sm->AddState(new StateHungry("HUNGRY", go));
	//	go->sm->AddState(new StateDead("DEAD", go));
	//}
	return FetchGO(type);
}

void SceneMovement::Update(double dt)
{
	SceneBase::Update(dt);

	SpawnTime -= dt * SceneData::GetInstance()->GetSpeed();
	if (SpawnTime <= 0.0)
	{
		SpawnTime = FixSpawnTime;
		int random = Math::RandIntMinMax(3, 10);

		for(int i = 0; i < random; ++i)
			FetchGO("Customer");
	}

	static const float BALL_SPEED = 5.f;
	static const float HOUR_SPEED = 1.f;

	//Calculating aspect ratio
	m_worldHeight = 100.f;
	m_worldWidth = m_worldHeight * (float)Application::GetWindowWidth() / Application::GetWindowHeight();
	
	if(Application::IsKeyPressed(VK_OEM_MINUS))
	{
		SceneData::GetInstance()->SetSpeed(Math::Max(0.f, SceneData::GetInstance()->GetSpeed() - 0.1f));
	}
	if(Application::IsKeyPressed(VK_OEM_PLUS))
	{
		SceneData::GetInstance()->AddSpeed(0.1f);
	}

	m_hourOfTheDay += HOUR_SPEED * static_cast<float>(dt) * SceneData::GetInstance()->GetSpeed();
	if (m_hourOfTheDay >= 24.f)
		m_hourOfTheDay = 0;

	//Input Section
	static bool bLButtonState = false;
	if(!bLButtonState && Application::IsMousePressed(0))
	{
		bLButtonState = true;
		std::cout << "LBUTTON DOWN" << std::endl;
	}
	else if(bLButtonState && !Application::IsMousePressed(0))
	{
		bLButtonState = false;
		std::cout << "LBUTTON UP" << std::endl;
	}
	static bool bRButtonState = false;
	if(!bRButtonState && Application::IsMousePressed(1))
	{
		bRButtonState = true;
		std::cout << "RBUTTON DOWN" << std::endl;
	}
	else if(bRButtonState && !Application::IsMousePressed(1))
	{
		bRButtonState = false;
		std::cout << "RBUTTON UP" << std::endl;
	}
	static bool bSpaceState = false;
	if (!bSpaceState && Application::IsKeyPressed(VK_SPACE))
	{
		bSpaceState = true;
		FetchGO("Customer");
	}
	else if (bSpaceState && !Application::IsKeyPressed(VK_SPACE))
	{
		bSpaceState = false;
	}
	static bool bVState = false;
	if (!bVState && Application::IsKeyPressed('V'))
	{
		bVState = true;
	}
	else if (bVState && !Application::IsKeyPressed('V'))
	{
		bVState = false;
	}

	//StateMachine
	StateMachineManager::GetInstance()->Update(dt);

	//Movement Section
	for(std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject *go = (GameObject *)*it;
		if (!go->GetActive())
			continue;
		Vector3 dir = go->GetTarget() - go->GetPosition();
		if (dir.Length() < go->GetMoveSpeed() * dt * SceneData::GetInstance()->GetSpeed())
		{
			//GO->pos reach target
			go->SetPosition(go->GetTarget());
			if (go->GetMoveRight())
				go->SetTarget(go->GetPosition() + Vector3(SceneData::GetInstance()->GetGridSize(), 0.f, 0.f));
			else if (go->GetMoveLeft())
				go->SetTarget(go->GetPosition() + Vector3(-SceneData::GetInstance()->GetGridSize(), 0.f, 0.f));
			else if (go->GetMoveUp())
				go->SetTarget(go->GetPosition() + Vector3(0.f, SceneData::GetInstance()->GetGridSize(), 0.f));
			else if(go->GetMoveDown())
				go->SetTarget(go->GetPosition() + Vector3(0.f, -SceneData::GetInstance()->GetGridSize(), 0.f));
	  //    //Boundary check
			//if (go->GetTarget().x < 0 || go->GetTarget().x > SceneData::GetInstance()->GetNoGrid() * SceneData::GetInstance()->GetGridSize() || go->GetTarget().y < 0 || go->GetTarget().y > SceneData::GetInstance()->GetNoGrid() * SceneData::GetInstance()->GetGridSize())
			//	go->SetTarget(go->GetPosition());
		}
		else
		{
			try
			{
				dir.Normalize();
				go->SetPosition(go->GetPosition() += dir * go->GetMoveSpeed() * static_cast<float>(dt) * SceneData::GetInstance()->GetSpeed());
			}
			catch (DivideByZero)
			{
			}
		}
	}
	////Counting objects
	//m_numGO[GameObject::GO_FISH] = m_numGO[GameObject::GO_SHARK] = m_numGO[GameObject::GO_FISHFOOD] = 0;
	//for (std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	//{
	//	GameObject *go = (GameObject *)*it;
	//	if (!go->active)
	//		continue;
	//	++m_numGO[go->type];
	//}
}

bool SceneMovement::Handle(Message *message)
{
	delete message;
	return false;
}

void SceneMovement::RenderGO(GameObject *go)
{
	//if (go->GetType() == "Customer")
	//{
	//	Customer *customer = dynamic_cast<Customer*>(go);
	//	modelStack.PushMatrix();
	//	modelStack.Translate(go->GetPosition().x, go->GetPosition().y, zOffset);
	//	modelStack.Scale(go->GetScale().x, go->GetScale().y, go->GetScale().z);
	//	if (go->GetCurrentState()->GetStateID() == "Walk")
	//		RenderMesh(meshList[GEO_TOOFULL], false);
	//	else if (go->GetCurrentState()->GetStateID() == "Decide")
	//		RenderMesh(meshList[GEO_DECIDE], false);
	//	else if (go->GetCurrentState()->GetStateID() == "Go table" && customer->GetBoughtFood())
	//		RenderMesh(meshList[GEO_EAT], false);
	//	else if (go->GetCurrentState()->GetStateID() == "Go table")
	//		RenderMesh(meshList[GEO_GOTABLE], false);
	//	else if (go->GetCurrentState()->GetStateID() == "Order food")
	//		RenderMesh(meshList[GEO_ORDERFOOD], false);		
	//	modelStack.PopMatrix();
	//}
}

void SceneMovement::Render()
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
	RenderMesh(meshList[GEO_BG], false);
	modelStack.PopMatrix();

	zOffset = 0;
	for(std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject *go = (GameObject *)*it;
		if(go->GetActive())
		{
			zOffset += 0.001f;
			RenderGO(go);
		}
	}

	//On screen text

	std::ostringstream ss;
	ss.precision(3);
	ss << "Speed:" << SceneData::GetInstance()->GetSpeed();
	RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 50, 6);
	
	ss.str("");
	ss.precision(5);
	ss << "FPS:" << fps;
	RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 50, 3);

	//ss.str("");
	//ss << "Fishes:" << m_numGO[GameObject::GO_FISH];
	//RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 50, 18);
	//ss.str("");
	//ss << "Shark:" << m_numGO[GameObject::GO_SHARK];
	//RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 50, 15);
	//ss.str("");
	//ss << "Food:" << m_numGO[GameObject::GO_FISHFOOD];
	//RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 50, 12);
	
	RenderTextOnScreen(meshList[GEO_TEXT], "Coffee Shop", Color(0, 1, 0), 3, 50, 0);
}

void SceneMovement::Exit()
{
	SceneBase::Exit();
	//Cleanup GameObjects
	while(m_goList.size() > 0)
	{
		GameObject *go = m_goList.back();
		delete go;
		m_goList.pop_back();
	}
	if(m_ghost)
	{
		delete m_ghost;
		m_ghost = NULL;
	}
	StateMachineManager::GetInstance()->CleanManager();
}
