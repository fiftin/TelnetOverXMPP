#include "connectionbase.h"
#include <qfile.h>
#include <qfileinfo.h>
#include "filemessage.h"

#include <interfaces/ifiletransfer.h>
#include <interfaces/ifilestreamsmanager.h>
#include <definitions/optionvalues.h>
#include <definitions/namespaces.h>
#include <definitions/internalerrors.h>

ConnectionBase::ConnectionBase(IMessageSender *AMessageSender, IFileTransfer *AFileTransfer,
                               IFileStreamsManager *AFileManager,
                               const QString& ASid, const QString& ARemoteSid, const Jid& AJid, const Jid& ARemoteJid, QObject *parent)
    : QObject(parent),
      FMessageSender(AMessageSender),
      FFileTransfer(AFileTransfer),
      FFileManager(AFileManager),
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

bool ConnectionBase::fileStreamResponce(const QString &AStreamId, const Stanza &AResponce, const QString &AMethodNS)
{
    if (FFileManager!=NULL && FFileManager->streamHandler(AStreamId)==this)
    {
        IFileStream *stream = FFileManager->streamById(AStreamId);
        QDomElement rangeElem = AResponce.firstElement("si",NS_STREAM_INITIATION).firstChildElement("file").firstChildElement("range");
        if (!rangeElem.isNull())
        {
            if (rangeElem.hasAttribute("offset"))
                stream->setRangeOffset(rangeElem.attribute("offset").toLongLong());
            if (rangeElem.hasAttribute("length"))
                stream->setRangeLength(rangeElem.attribute("length").toLongLong());
        }
        if (!stream->startStream(AMethodNS))
            stream->abortStream(XmppError(IERR_FILETRANSFER_TRANSFER_NOT_STARTED));
        else
            return true;
    }
    return false;
}

bool ConnectionBase::sendBigFile(const QString &APathname, bool AForce) {
    IFileStream *stream;
    if (!AForce) {
        stream = FFileTransfer->sendFile(this->info().jid, this->info().remoteJid, APathname);
    }
    else {
        if (APathname == QString::null)
            return false;
        QString defaultMethod = Options::node(OPV_FILESTREAMS_DEFAULTMETHOD).value().toString();
        stream = FFileManager->createStream(this, QUuid::createUuid().toString(), this->info().jid,
                                            this->info().remoteJid, IFileStream::SendFile);
        stream->setFileName(APathname);
        QStringList methods;
        methods.append(defaultMethod);
        if (!stream->initStream(methods)) {
            return false;
        }
    }
    QString filename;
    if (APathname != QString::null) {
        QFile* file = new QFile(APathname);
        if (file->exists()) {
            QFileInfo fileInfo(APathname);
            filename = fileInfo.fileName();
        }
        file->close();
    }
    this->send(FileMessage::createFile(this->info(), filename, stream->streamId(), FMM_FILETRANSFER));
    return true;
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
