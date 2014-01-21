#include "remoteterminalmessage.h"

RemoteTerminalMessage::RemoteTerminalMessage(const Stanza& AStanza)
    : Message2(AStanza)
{
}

RemoteTerminalMessage::RemoteTerminalMessage(const Message2& AMessage)
    : Message2(AMessage.stanza())
{
}

RemoteTerminalMessage::RTMType RemoteTerminalMessage::rtmType() const
{
    if (stanza().element().attribute("rtmType") == "stdout")
        return rtmStdout;
    else if (stanza().element().attribute("rtmType") == "stderr")
        return rtmStderr;
    else if (stanza().element().attribute("rtmType") == "error")
        return rtmError;
    else
        return rtmUnknown;
}

int RemoteTerminalMessage::errorCode() const
{
    return stanza().element().attribute("rtmErrorCode").toInt();
}
