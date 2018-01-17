#include "StateMachineManager.h"
#include <algorithm>

void StateMachineManager::CleanManager(void)
{
	for (std::multimap<std::string, State*>::iterator it = m_stateMap.begin(); it != m_stateMap.end(); ++it)
		delete it->second;
	m_stateMap.clear();
}

void StateMachineManager::AddState(std::string type, State *newState)
{
	// Will not have any check, must add States in init only, if go next scene need to clear 
	m_stateMap.insert(std::pair<std::string, State*>(type, newState));
}

void StateMachineManager::SetNextState(GameObject *go, std::string nextState)
{
	std::pair<std::multimap<std::string, State*>::iterator, std::multimap<std::string, State*>::iterator> ret;
	ret = m_stateMap.equal_range(go->GetType());

	for (std::multimap<std::string, State*>::iterator it = ret.first; it != ret.second; ++it)
	{
		if (it->second->GetStateID() != nextState)
			continue;

		go->SetNextState(it->second);
		break;
	}
}

// Set default state when first created GameObject
void StateMachineManager::DefaultState(GameObject *go)
{
	// Check if there are registered m_stateMap key values
	if (m_stateMap.find(go->GetType()) == m_stateMap.end())
		return;
	std::pair<std::multimap<std::string, State*>::iterator, std::multimap<std::string, State*>::iterator> ret;
	ret = m_stateMap.equal_range(go->GetType());

	for (std::multimap<std::string, State*>::iterator it = ret.first; it != ret.second; ++it)
	{
		go->SetCurrentState(it->second);
		go->SetNextState(it->second);
		go->GetCurrentState()->Enter(go);
		break;
	}
}

// Add GameObjects into m_goList
void StateMachineManager::AddGameObject(GameObject *go)
{
	m_goList.push_back(go);
}

// Remove GameObject from m_goList when inactive
void StateMachineManager::RemoveGameObject(GameObject *go)
{
	m_goList.erase(std::remove(m_goList.begin(), m_goList.end(), go), m_goList.begin());
}

void StateMachineManager::Update(double dt)
{
	for (std::vector<GameObject*>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject *go = (GameObject *)*it;
		if (!go->GetActive())
			continue;
		if (go->GetCurrentState() != go->GetNextState())
		{
			go->GetCurrentState()->Exit(go);
			go->SetCurrentState(go->GetNextState());
			go->GetCurrentState()->Enter(go);
		}
		go->GetCurrentState()->Update(dt, go);
	}
}