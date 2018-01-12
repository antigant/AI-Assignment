#include "Message.h"

Message::Message(std::string messageFrom, std::string messageID)
	: messageFrom(messageFrom)
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

std::string Message::GetMessageID(void) const
{
	return messageID;
}

// Setter
void Message::SetMessageFrom(const std::string messageFrom)
{
	this->messageFrom = messageFrom;
}

void Message::SetMessageID(const std::string messageID)
{
	this->messageID = messageID;
}