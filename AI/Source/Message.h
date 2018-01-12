#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>

class Message
{
public:
	Message(std::string messageFrom = "No one", std::string messageID = "No ID");
	virtual ~Message();

	// Getter
	std::string GetMessageFrom(void) const;
	std::string GetMessageID(void) const;

	// Setter
	void SetMessageFrom(const std::string messageFrom);
	void SetMessageID(const std::string messageID);

protected:
	// GameObject type
	std::string messageFrom;
	// GameObject id
	std::string messageID;
};

#endif