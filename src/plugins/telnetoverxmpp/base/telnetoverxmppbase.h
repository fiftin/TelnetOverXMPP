#ifndef TELNETOVERXMPPBASE_H
#define TELNETOVERXMPPBASE_H
#include "connectionbase.h"
#include <QMap>
#include "interfaces/istanzaprocessor.h"


class TelnetOverXmppBase
{
public:
    TelnetOverXmppBase() { }

    ~TelnetOverXmppBase() {
        removeAllConnections();
    }

    void removeAllConnections();

    IStanzaHandle makeStanzaHandle(IStanzaHandler *AHandler) const;

    bool tryHandleMessage(const Message2& AMessage) {
        Message2 errMsg;
        return tryHandleMessage(AMessage, errMsg);
    }

    bool tryHandleMessage(const Message2& AMessage, Message2& AErrorMessage);

    void addConnection(ConnectionBase* AConnection);

    void removeConnection(ConnectionBase* AConnection);

    void removeConnection(const QString& ASid) {
        FConnections.remove(ASid);
    }

    QList<ConnectionBase*> getConnections() const {
        return FConnections.values();
    }

    ConnectionBase* getConnection(const QString& ASid) const {
        return FConnections.value(ASid, NULL);
    }

    QList<ConnectionBase*> removeClosedConnections();

private:
    QMap<QString, ConnectionBase*> FConnections;
};

#endif // TELNETOVERXMPPBASE_H
