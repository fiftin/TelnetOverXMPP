#ifndef PWDMESSAGE_H
#define PWDMESSAGE_H
#include "message2.h"
class PWDMessage : public Message2
{
public:
    PWDMessage();
    PWDMessage(const Message2& AMessage);
    static bool isPWD(const Message2& AMessage);
    static PWDMessage createMessage(const ConnectionInfo& AInfo, const QString &APWD);
};

#endif // PWDMESSAGE_H
