#include "SceneData.h"

void SceneData::Init(void)
{
}

// Getters
int SceneData::GetObjectCount(void) const
{
	return m_objectCount;
}

int SceneData::GetNoGrid(void) const
{
	return m_noGrid;
}

float SceneData::GetGridSize(void) const
{
	return m_gridSize;
}

float SceneData::GetGridOffSet(void) const
{
	return m_gridOffset;
}

float SceneData::GetSpeed(void) const
{
	return m_speed;
}

// Setter
void SceneData::SetObjectCount(const int m_objectCount)
{
	this->m_objectCount = m_objectCount;
}

void SceneData::SetNoGrid(const int m_noGrid)
{
	this->m_noGrid = m_noGrid;
}

void SceneData::SetGridSize(const float m_gridSize)
{
	this->m_gridSize = m_gridSize;
}

void SceneData::SetGridOffset(const float m_gridOffset)
{
	this->m_gridOffset = m_gridOffset;
}

void SceneData::SetSpeed(const float m_speed)
{
	this->m_speed = m_speed;
}

// Sorta Math thing
void SceneData::AddObjectCount(const int amount)
{
	m_objectCount += amount;
}

void SceneData::MinusObjectCount(const int amount)
{
	m_objectCount -= amount;
}

void SceneData::AddSpeed(const float m_speed)
{
	this->m_speed += m_speed;
}

void SceneData::MinusSpeed(const float m_speed)
{
	this->m_speed -= m_speed;
}

SceneData::SceneData()
	: m_objectCount(0)
	, m_noGrid(0)
	, m_gridSize(0.f)
	, m_gridOffset(0.f)
	, m_speed(1.f)
{
}

SceneData::~SceneData()
{
}