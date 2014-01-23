#ifndef CONTROLCMESSAGE_H
#define CONTROLCMESSAGE_H
#include "message2.h"
class ControlCMessage : public Message2
{
public:
    ControlCMessage();
    ControlCMessage(const Message2& AMessage);
    static ControlCMessage createMessage(const ConnectionInfo& AInfo);
    static bool isControlCMessage(const Message2& AMessage);
};

#endif // CONTROLCMESSAGE_H
