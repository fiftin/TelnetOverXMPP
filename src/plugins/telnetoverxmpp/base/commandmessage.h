#ifndef COMMANDMESSAGE_H
#define COMMANDMESSAGE_H

#include "message2.h"

class CommandMessage : public Message2
{
public:
    CommandMessage();
    CommandMessage(const Message2& AMessage);
    static bool isCommand(const Message2& AMessage);
    static CommandMessage createMessage(const ConnectionInfo& AInfo, const QString &ACommand);
};

#endif // COMMANDMESSAGE_H
