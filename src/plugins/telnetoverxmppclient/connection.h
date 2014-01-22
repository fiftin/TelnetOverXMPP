#ifndef CONNECTION_H
#define CONNECTION_H


#include "../telnetoverxmpp/base/connectionbase.h"


class Connection : public ConnectionBase
{
    Q_OBJECT
public:

    Connection(IMessageSender* AMessageSender, IFileTransfer *AFileTransfer,
               IFileStreamsManager *AFileManager, const Jid& AJid, const Jid& ARemoteJid, bool AOpen=true);

    ~Connection() { }

    void open();

public:

    virtual void handleMessage(const Message2& AMessage);

    void sendControlC() const;

    QString connectionRequestId() const {
        return FConnectionRequestId;
    }

private:
    QString FConnectionRequestId;
};

#endif // CONNECTION_H
