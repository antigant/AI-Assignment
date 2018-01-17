#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>

class Message
{
public:
	Message(std::string messageFrom = "No one", std::string messageType = "No Type", std::string messageID = "No ID");
	virtual ~Message();

	// Getter
	std::string GetMessageFrom(void) const;
	std::string GetMessageID(void) const;
	std::string GetMessageType(void) const;

	// Setter
	void SetMessageFrom(const std::string messageFrom);
	void SetMessageType(const std::string messageType);
	void SetMessageID(const std::string messageID);

protected:
	// GameObject type
	std::string messageFrom;
	// What is the message trying to say eg. My turn, your turn etc..
	std::string messageType;
	// GameObject ID
	std::string messageID;
};

#endif