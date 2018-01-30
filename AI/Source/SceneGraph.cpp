#include "SceneGraph.h"
#include "GL\glew.h"
#include "Application.h"
#include <sstream>

SceneGraph::SceneGraph()
{
}

SceneGraph::~SceneGraph()
{
}

void SceneGraph::Init()
{
	SceneBase::Init();

	//Calculating aspect ratio
	m_worldHeight = 100.f;
	m_worldWidth = m_worldHeight * (float)Application::GetWindowWidth() / Application::GetWindowHeight();

	//Physics code here
	m_speed = 1.f;

	Math::InitRNG();

	m_graph.Generate(0, m_worldHeight);
	InitPath();
}

GameObject* SceneGraph::FetchGO(std::string type)
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
	//for (unsigned i = 0; i < 5; ++i)
	//{
		GameObject *go = new GameObject(type);
		m_goList.push_back(go);
	//}
	return FetchGO(type);
}

void SceneGraph::Update(double dt)
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
		if (posX < m_worldHeight && posY < m_worldHeight)
		{
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
		go->SetPosition(Vector3(Math::RandFloatMinMax(0, m_worldWidth), Math::RandFloatMinMax(0, m_worldHeight)));
		go->SetTarget(go->GetPosition());
		go->currNodeID = m_graph.NearestNode(go->GetPosition());
		go->gStack.clear();
		go->gStack.push_back(go->currNodeID);
		go->visited.resize(m_graph.m_nodes.size(), false);
	}
	else if (bSpaceState && !Application::IsKeyPressed(VK_SPACE))
	{
		bSpaceState = false;
	}

	static const float NPC_VELOCITY = 10.f;
	for (auto go : m_goList)
	{
		if (!go->GetActive() && go->GetType() != "NPC")
			continue;
		if ((go->GetPosition() - go->GetTarget()).Length() < 1.f)
		{
			if (!go->gStack.empty())
			{
				DFSOnce(go);
				go->SetTarget(m_graph.m_nodes[go->currNodeID]->pos);
			}
			else if (!go->gPath.empty())
			{
				go->SetTarget(go->gPath[0]);
				go->gPath.erase(go->gPath.begin());
			}
			else
				GoRandomAdjacent(go);
		}
		else
			go->SetPosition(go->GetPosition() += (go->GetTarget() - go->GetPosition()).Normalized() * NPC_VELOCITY * m_speed * static_cast<float>(dt));
	}
}

void SceneGraph::RenderGO(GameObject *go)
{
	if (go->GetType() == "NPC")
	{
		modelStack.PushMatrix();
		modelStack.Translate(go->GetPosition().x, go->GetPosition().y, 2.f);
		modelStack.Scale(5.f, 5.f, 5.f);
		RenderMesh(meshList[GEO_QUEEN], false);
		modelStack.PopMatrix();
	}
}

void SceneGraph::RenderGraph()
{
	// Rendering the nodes
	for (int i = 0; i < m_graph.m_nodes.size(); ++i)
	{
		modelStack.PushMatrix();
		modelStack.Translate(m_graph.m_nodes[i]->pos.x, m_graph.m_nodes[i]->pos.y, m_graph.m_nodes[i]->pos.z);
		RenderMesh(meshList[GEO_NODE], false);
		modelStack.PopMatrix();
	}

	// Rendering the edges
	for (int i = 0; i < m_graph.m_edges.size(); ++i)
	{
		Node *from = m_graph.m_nodes[m_graph.m_edges[i]->from];
		Node *to = m_graph.m_nodes[m_graph.m_edges[i]->to];
		float theta = Math::RadianToDegree(atan2(to->pos.y - from->pos.y, to->pos.x - from->pos.x));

		modelStack.PushMatrix();
		modelStack.Translate(from->pos.x, from->pos.y, from->pos.z);
		modelStack.Rotate(theta, 0.f, 0.f, 1.f);
		modelStack.Scale(m_graph.m_edges[i]->cost, 1.f, 1.f);
		RenderMesh(meshList[GEO_EDGE], false);
		modelStack.PopMatrix();
	}
}

void SceneGraph::GoRandomAdjacent(GameObject *go)
{
	// Neighbour Node algorithm
	unsigned nodeID = m_graph.NearestNode(go->GetPosition());
	Node *currNode = m_graph.m_nodes[nodeID];

	unsigned RNG = Math::RandIntMinMax(0, currNode->edges.size() - 1);
	unsigned edgeID = currNode->edges[RNG];
	Edge *edge = m_graph.m_edges[edgeID];
	Node *nextNode = m_graph.m_nodes[edge->to];
	// end of Neighbour Node algorithm
	go->gPath.push_back(nextNode->pos);
}

void SceneGraph::DFSOnce(GameObject *go)
{
	//go->gStack.push_back(go->currNodeID);
	//go->visited[go->currNodeID] = true;

	//// Check neighbour nodes
	//for (auto neighbour : m_graph.m_nodes)
	//{

	//}
}

bool SceneGraph::AStar(GameObject *go, unsigned start, unsigned end)
{
	//std::vector<bool> visited(m_graph.m_nodes.size(), false);
	//std::queue<QueueNode> queue;
	//go->gPath.clear();

	//queue.push(QueueNode(start, 0));
	//while (!queue.empty())
	//{
	//	unsigned curr = queue.front().node;
	//	QueueNode parent = queue.front();
	//	queue.pop();

	//	Node *currNode = m_graph.m_nodes[curr];
	//	QueueNode currQueueNode;
	//	currQueueNode.node = curr;

	//	currQueueNode.cost = 0;
	//}

	return false;
}

void SceneGraph::InitPath()
{

}


void SceneGraph::AssignPath(GameObject *go)
{

}

void SceneGraph::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Projection matrix : Orthographic Projection
	Mtx44 projection;
	projection.SetToOrtho(0, m_worldWidth, 0, m_worldHeight, -10, 10);
	projectionStack.LoadMatrix(projection);

	RenderGraph();

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

	//Render m_goList
	for (std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject *go = (GameObject *)*it;
		if (go->GetActive())
			RenderGO(go);
	}

	modelStack.PushMatrix();
	modelStack.Translate(m_worldHeight * 0.5f, m_worldHeight * 0.5f, -1.f);
	modelStack.Scale(m_worldHeight, m_worldHeight, m_worldHeight);
	RenderMesh(meshList[GEO_WHITEQUAD], false);
	modelStack.PopMatrix();

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
	ss << "Graph " << 0;
	RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 50, 0);
}

void SceneGraph::Exit()
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
