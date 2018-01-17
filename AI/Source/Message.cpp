#include "Message.h"

Message::Message(std::string messageFrom, std::string messageType, std::string messageID)
	: messageFrom(messageFrom)
	, messageType(messageType)
	, messageID(messageID)
{

}

Message::~Message()
{

}

// Getter
std::string Message::GetMessageFrom(void) const
{
	return messageFrom;
}

std::string Message::GetMessageType(void) const
{
	return messageType;
}

// Setter
void Message::SetMessageFrom(const std::string messageFrom)
{
	this->messageFrom = messageFrom;
}

void Message::SetMessageType(const std::string messageType)
{
	this->messageType = messageType;
}

void Message::SetMessageID(const std::string messageID)
{
	this->messageID = messageID;
}