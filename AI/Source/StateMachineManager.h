#ifndef	STATE_MACHINE_MANAGER_H
#define STATE_MACHINE_MANAGER_H

#include "SingletonTemplate.h"
#include "State.h"
#include "GameObject.h"

#include <map>
#include <string>
#include <vector>

// Manage all the state
class StateMachineManager : public Singleton<StateMachineManager>
{
	friend Singleton<StateMachineManager>;
public:
	//~StateMachineManager();
	// Clear States in m_stateMap
	void CleanManager(void);
	void AddState(std::string type, State *newState = NULL);
	void SetNextState(GameObject *go, std::string nextState);
	// Set default state when first created GameObject
	void DefaultState(GameObject *go);
	// Add GameObjects into m_goList
	void AddGameObject(GameObject *go);
	// Remove GameObject from m_goList when inactive
	void RemoveGameObject(GameObject *go);

	void Update(double dt);

private:
	// key value : GameObject type, map value : State
	std::multimap<std::string, State*> m_stateMap;
	std::vector<GameObject*> m_goList;
};

#endif