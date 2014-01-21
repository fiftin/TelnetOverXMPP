#include "telnetoverxmppbase.h"
#include "exception.h"


void TelnetOverXmppBase::removeAllConnections() {
    foreach (ConnectionBase* c, FConnections) {
        c->close();
        //delete c;
    }
    FConnections.clear();
}

IStanzaHandle TelnetOverXmppBase::makeStanzaHandle(IStanzaHandler *AHandler) const {
    IStanzaHandle handle;
    handle.conditions.append("/iq/[@xmlns='"+TNOXMPP_XMLNS+"']");
    handle.conditions.append("/iq/connect[@xmlns='"+TNOXMPP_XMLNS+"']");
    handle.conditions.append("/iq/connected[@xmlns='"+TNOXMPP_XMLNS+"']");
    handle.conditions.append("/iq/error[@xmlns='"+TNOXMPP_XMLNS+"']");
    handle.conditions.append("/iq/disconnect[@xmlns='"+TNOXMPP_XMLNS+"']");
    handle.conditions.append("/iq/disconnected[@xmlns='"+TNOXMPP_XMLNS+"']");
    handle.conditions.append("/message/[@xmlns='"+TNOXMPP_XMLNS+"']");
    handle.conditions.append("/message/data[@xmlns='"+TNOXMPP_XMLNS+"']");
    handle.conditions.append("/message/terminated[@xmlns='"+TNOXMPP_XMLNS+"']");

    handle.direction = IStanzaHandle::DirectionIn;
    handle.handler = AHandler;
    return handle;
}

bool TelnetOverXmppBase::tryHandleMessage(const Message2& AMessage, Message2& AErrorMessage) {
    QString sid = AMessage.toSid();
    ConnectionBase *conn = getConnection(sid);
    AErrorMessage = Message2::createEmpty();
    if (conn == NULL || !conn->isMessageForMe(AMessage)) {
        return false;
    //    errorCode = EMC_SESSION_ID_IS_NOT_EXISTS;
    //    errorMessage = "Session not exists";
    //}
    //else if (!conn->isMessageForMe(AMessage)) {
    //    errorCode = EMC_STANZA_IS_NOT_FOR_ME;
    //    errorMessage = "";
    }
    else {
        try {
            conn->handleMessage(AMessage);
            return true;
        }
        catch(...) {
            return false;
            //errorCode = EMC_EXCEPTION;
            //errorMessage = "";
        }
    }
    //AErrorMessage = Message2::createErrorMessage(
    //    ConnectionInfo(AMessage.toSid(), AMessage.fromSid(), AMessage.to(), AMessage.from()), 0, "");
    return false;
}

QList<ConnectionBase *> TelnetOverXmppBase::removeClosedConnections()
{
    QList<ConnectionBase*> ret;
    QMap<QString, ConnectionBase*>::iterator it = FConnections.begin();
    while (it != FConnections.begin()) {
        if (it.value()->state() == CS_CLOSED) {
            FConnections.remove(it.key());
            ret.append(it.value());
        }
    }
    return ret;
}

void TelnetOverXmppBase::addConnection(ConnectionBase* AConnection) {
    FConnections.insert(AConnection->sid(), AConnection);
}

void TelnetOverXmppBase::removeConnection(ConnectionBase* AConnection) {
    removeConnection(AConnection->sid());
}
