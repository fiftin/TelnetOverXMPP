#ifndef CONNECTION_H
#define CONNECTION_H


#include "base/connectionbase.h"


class Connection : public ConnectionBase
{
    Q_OBJECT
public:

    Connection(IMessageSender* AMessageSender, const Jid& AJid, const Jid& ARemoteJid, bool AOpen=true);
    ~Connection() {
        qDebug() << "~Connection()";
    }

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
