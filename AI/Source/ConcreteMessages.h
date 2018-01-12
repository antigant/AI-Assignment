#ifndef CONCRETE_MESSAGE_H
#define CONCRETE_MESSAGE_H

#include "Message.h"
#include "GameObject.h"

struct MessageWRU : public Message
{
	//enum SEARCH_TYPE
	//{
	//	SEARCH_NONE = 0,
	//	NEAREST_SHARK,
	//	NEAREST_FISHFOOD,
	//	NEAREST_FULLFISH,
	//	HIGHEST_ENERGYFISH,
	//};

	MessageWRU(GameObject *goValue, std::string searchType, float thresholdValue) : go(goValue), searchType(searchType), threshold(thresholdValue) {}
	MessageWRU(GameObject *goValue, std::string searchType) : go(goValue), searchType(searchType) {}
	virtual ~MessageWRU() {}

	GameObject *go;
	std::string searchType;
	float threshold;
};

#endif
