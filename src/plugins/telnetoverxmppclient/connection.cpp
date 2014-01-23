#include "connection.h"

#include <interfaces/ifiletransfer.h>
#include <interfaces/ifilestreamsmanager.h>
#include "../telnetoverxmpp/base/controlcmessage.h"

Connection::Connection(IMessageSender* AMessageSender, IFileTransfer *AFileTransfer,
                       IFileStreamsManager *AFileManager,
                       const Jid& AJid, const Jid& ARemoteJid, bool AOpen)
    : ConnectionBase(AMessageSender, AFileTransfer, AFileManager, makeNewSid(), "", AJid, ARemoteJid)
{
    if (AOpen)
        open();
}

void Connection::open()
{
    if (state() == CS_NONE) {
        Message2 msg = Message2::createSessionOpenReq(info());
        request(msg, 1000);
        QString id = msg.id();
        FConnectionRequestId = id;

    }
    if (state() == CS_CLOSED)
        throw Exception("Can't open closed connection");
}

void Connection::handleMessage(const Message2 &AMessage)
{
    if (AMessage.isResult()) {
        if (AMessage.id() == FConnectionRequestId) {
            if (AMessage.isError()) {
                onConnectionError(ConnectionError(CEC_UNKNOWN, AMessage.errorMessage()));
            }
            else {
                setRemoteSid(AMessage.fromSid());
                onConnected();
            }
        }
    }
    ConnectionBase::handleMessage(AMessage);
}

void Connection::sendControlC() const
{
    send(ControlCMessage::createMessage(getInfo()));
}

