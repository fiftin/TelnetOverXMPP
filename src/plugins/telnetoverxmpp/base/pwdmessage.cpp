#include "pwdmessage.h"

PWDMessage::PWDMessage()
{
}

PWDMessage::PWDMessage(const Message2 &AMessage)
    : Message2(AMessage.stanza())
{
}

bool PWDMessage::isPWD(const Message2& AMessage)
{
    return AMessage.isDataMessage() &&  AMessage.attr("type") == "pwd";
}

PWDMessage PWDMessage::createMessage(const ConnectionInfo& AInfo, const QString &APWD)
{
    PWDMessage result(Message2::createDataMessage(AInfo, APWD));
    result.setAttr("type", "pwd");
    return result;
}
