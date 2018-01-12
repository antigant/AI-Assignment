#ifndef SCENE_DATA_H
#define SCENE_DATA_H

#include "SingletonTemplate.h"

class SceneData : public Singleton<SceneData>
{
	friend Singleton<SceneData>;
public:
	void Init(void);

	// Getters
	int GetObjectCount(void) const;
	int GetNoGrid(void) const;
	float GetGridSize(void) const;
	float GetGridOffSet(void) const;
	float GetSpeed(void) const;

	// Setters
	void SetObjectCount(const int m_objectCount);
	void SetNoGrid(const int m_noGrid);
	void SetGridSize(const float m_gridSize);
	void SetGridOffset(const float m_gridOffset);
	void SetSpeed(const float m_speed);

	// Sorta Math thing
	void AddObjectCount(const int amount);
	void MinusObjectCount(const int amount);
	void AddSpeed(const float m_speed);
	void MinusSpeed(const float m_speed);

private:
	SceneData();
	~SceneData();
	int m_objectCount;
	int m_noGrid;
	float m_gridSize;
	float m_gridOffset;
	float m_speed;			// Use this to determine the speed of everything
};

#endif