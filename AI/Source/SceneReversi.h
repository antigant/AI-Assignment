#ifndef SCENE_REVERSI_H
#define SCENE_REVERSI_H

#include "GameObject.h"
#include <vector>
#include "SceneBase.h"

class SceneReversi : public SceneBase
{
	enum WHO_CONTENT
	{
		WHO_NONE = 0,
		WHO_BLACK,
		WHO_WHITE,
	};
public:
	SceneReversi();
	~SceneReversi();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();

	void RenderGO(GameObject *go);

	GameObject* FetchGO(const std::string type);

	int GetAIDecision();

	void Reset();

private:
	// Lets put our helper functions here
	bool IsInBoard(int posX, int poxY);
	int ConvertXYPos(int posX, int posY);
	void ConvertArrayPos(int pos, int &posX, int &posY);

	bool IsLegalMove(int posX, int posY, bool flip = false);

	bool CheckSlot(int posX, int posY, int dirX, int dirY, int count, bool flip = false);

protected:

	std::vector<GameObject *> m_goList;
	float m_speed;
	float m_worldWidth;
	float m_worldHeight;
	GameObject *m_ghost;
	WHO_CONTENT *m_grid;
	bool m_bBlackTurn;
	bool m_bGameOver;
	WHO_CONTENT m_winner;
};

#endif