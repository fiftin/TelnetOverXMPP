#include "controlcmessage.h"

ControlCMessage::ControlCMessage()
{
    setAttr("type", "control-c");
}

ControlCMessage::ControlCMessage(const Message2 &AMessage)
    : Message2(AMessage.stanza())
{
}

ControlCMessage ControlCMessage::createMessage(const ConnectionInfo &AInfo)
{
    return ControlCMessage(Message2(AInfo));
}

bool ControlCMessage::isControlCMessage(const Message2 &AMessage)
{
    return AMessage.isDataMessage() && AMessage.attr("type") == "control-c";
}
