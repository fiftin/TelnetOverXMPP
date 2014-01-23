#include "commandmessage.h"

CommandMessage::CommandMessage()
{
}

CommandMessage::CommandMessage(const Message2 &AMessage)
    : Message2(AMessage.stanza())
{
}

bool CommandMessage::isCommand(const Message2 &AMessage)
{
    return AMessage.isDataMessage() && AMessage.attr("type") == "command";
}

CommandMessage CommandMessage::createMessage(const ConnectionInfo &AInfo, const QString &ACommand)
{
    CommandMessage result(Message2::createDataMessage(AInfo, ACommand));
    result.setAttr("type", "command");
    return result;
}
