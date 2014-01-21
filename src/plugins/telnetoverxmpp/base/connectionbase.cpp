#include "connectionbase.h"
#include <qfile.h>
#include <qfileinfo.h>
#include "filemessage.h"

ConnectionBase::ConnectionBase(IMessageSender *AMessageSender, const QString& ASid, const QString& ARemoteSid, const Jid& AJid, const Jid& ARemoteJid, QObject *parent)
    : QObject(parent),
      FMessageSender(AMessageSender),
      FInfo(ASid, ARemoteSid, AJid, ARemoteJid),
      FState(CS_NONE),
      FPinging(false)
{
}


ConnectionBase::~ConnectionBase()
{
    close();
}


void ConnectionBase::handleMessage(const Message2& AMessage) {
    if (AMessage.isPingReq()) {
        send(Message2::createPingResult(info(), AMessage.data(), AMessage.id()));
    }
    else if (AMessage.isResult()) {
        if (AMessage.id() == FPingId) {
            if (AMessage.isError()) {
                onConnectionError(ConnectionError(CEC_PING_ERROR, AMessage.data()));
                onNoPingRelay();
            }
            else if (AMessage.data() != FPingData){
                onNoPingRelay();
            }
            FPingId = "";
            FPingData = "";
            FPinging = false;
        }
    }
    else if (AMessage.isClosedMessage()) {
        onClosed(CCR_REMORTECLOSE);
    }
    onReceived(AMessage);
}


QString ConnectionBase::makeNewSid()
{
    return Message2::makeNewId();
}


void ConnectionBase::ping()
{
    if (!FPinging) {
        FPinging = true;
        FPingId = QUuid::createUuid().toString();
        FPingData = QUuid::createUuid().toString();
        request(Message2::createPingReq(info(), FPingData), 1000);
    }
}

void ConnectionBase::onNoPingRelay()
{
    close();
}

void ConnectionBase::close() {
    if (state() == CS_CONNECTED) {
        send(Message2::createClosedMessage(info()));
        onClosed(CCR_CLOSEMETHOD);
        setState(CS_CLOSED);
    }
}

bool ConnectionBase::isMessageForMe(const Message2& AMessage) {
    return AMessage.to() == jid() && AMessage.from() == remoteJid();
}

bool ConnectionBase::sendFile(const QString &APathname) const
{
    bool result = false;
    QFile* file = new QFile(APathname);
    if (file->exists()) {
        QFileInfo fileInfo(APathname);
        if (file->open(QFile::ReadOnly)) {
            this->send(FileMessage::createFile(this->info(), fileInfo.fileName(), file->readAll()));
            file->close();
            result = true;
        }
    }
    return result;
}

bool ConnectionBase::sendBigFile(const QString &APathname) const {
    bool result = false;
    QFile* file = new QFile(APathname);
    if (file->exists()) {
        QFileInfo fileInfo(APathname);
        IFileStream *stream = FFileTransfer->sendFile(this->info().jid, this->info().remoteJid, APathname);
        this->send(FileMessage::createFile(this->info(), fileInfo.fileName(), stream->streamId(), FMM_FILETRANSFER));
        result = true;
    }
    file->close();
    return result;
}

void ConnectionBase::send(const Message2& AMessage) const {
    FMessageSender->send(jid(), AMessage);
}

void ConnectionBase::send(const QString& AData) const {
    FMessageSender->send(jid(), Message2::createDataMessage(info(), AData));
}

void ConnectionBase::request(const Message2& AMessage, int ATimeout) const {
    FMessageSender->request(jid(), AMessage, ATimeout);
}
void ConnectionBase::onClosed(ConnectionClosingReason AReason) {
    setState(CS_CLOSED);
    emit closed(AReason);
}

void ConnectionBase::onConnectionError(const ConnectionError &AError)
{
    emit connectionError(AError);
    switch (AError.code()) {
    case CEC_CONNECT_ERROR:
        onClosed(CCR_REMORTECLOSE);
        break;
    default:
        break;
    }
}
void ConnectionBase::onConnected() {
    setState(CS_CONNECTED);
    emit connected();
}
