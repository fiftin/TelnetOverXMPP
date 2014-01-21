#ifndef REMOTETERMINALMESSAGE_H
#define REMOTETERMINALMESSAGE_H

#include "../base/message.h"

class RemoteTerminalMessage : public Message2
{
public:
    enum RTMType {
        rtmStdout,
        rtmStderr,
        rtmError,
        rtmUnknown
    };

    RemoteTerminalMessage(const Stanza& AStanza);

    RemoteTerminalMessage(const Message2& AMessage);

    RTMType rtmType() const;
    int errorCode() const;
};
#endif // REMOTETERMINALMESSAGE_H
