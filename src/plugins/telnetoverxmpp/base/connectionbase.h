#ifndef CONNECTIONBASE_H
#define CONNECTIONBASE_H

#include <QObject>
#include <QUuid>
#include <QDebug>

#include "message2.h"
#include "exception.h"
#include <interfaces/ifilestreamsmanager.h>

enum ConnectionErrorCode {
    CEC_UNKNOWN,
    CEC_CONNECT_ERROR = 1,
    CEC_PING_ERROR = 2,
    CEC_OTHER_ERROR = 10000
};

class ConnectionError {
public:

    ConnectionError(ConnectionErrorCode ACode, QString ADescription=QString())
        : FCode(ACode),
          FDescription(ADescription)
    {
    }

    ConnectionErrorCode code() const { return FCode; }
    const QString& message() const { return FDescription; }
private:
    ConnectionErrorCode FCode;
    QString FDescription;
};

enum ConnectionClosingReason {
    CCR_CLOSEMETHOD,
    CCR_REMORTECLOSE
};

enum ConnectionState {
    CS_NONE,
    CS_CLOSED,
    CS_CONNECTED
};
class IFileTransfer;
class IFileStreamsManager;
class ConnectionBase : public QObject, public IFileStreamsHandler
{
    Q_OBJECT
    Q_INTERFACES(IFileStreamsHandler)
public:

    explicit ConnectionBase(IMessageSender *AMessageSender, IFileTransfer *AFileTransfer,
                            IFileStreamsManager *AFileManager,
                            const QString& ASid, const QString& ARemoteSid,
                            const Jid& AJid, const Jid& ARemoteJid, QObject *parent = 0);

    virtual ~ConnectionBase();
    const ConnectionInfo& info() const { return FInfo; }
    const Jid& jid() const { return FInfo.jid; }
    const Jid& remoteJid() const { return FInfo.remoteJid; }
    QString sid() const { return FInfo.sid; }
    ConnectionState state() const { return FState; }
    QString remoteSid() const { return FInfo.remoteSid; }

    void setRemoteSid(const QString& ASid) { FInfo.remoteSid = ASid; }


    bool isMessageForMe(const Message2& AMessage);

    bool sendFile(const QString& APathname) const;
    bool sendBigFile(const QString& APathname = QString::null, bool AForce = false);
    void send(const Message2& AMessage) const;
    void send(const QString& AData) const;
    void request(const Message2& AMessage, int ATimeout) const;
    void ping();
    void close();

    virtual void handleMessage(const Message2& AMessage);

    static QString makeNewSid();

    virtual bool fileStreamRequest(int AOrder, const QString &AStreamId, const Stanza &ARequest, const QList<QString> &AMethods) {
        return false;
    }

    virtual bool fileStreamResponce(const QString &AStreamId, const Stanza &AResponce, const QString &AMethodNS);
    virtual bool fileStreamShowDialog(const QString &AStreamId) {
        return false;
    }


signals:
    void closed(ConnectionClosingReason);
    void received(const Message2&);
    void connectionError(const ConnectionError&);
    void connected();
protected:
    void onNoPingRelay();
    virtual void onClosed(ConnectionClosingReason AReason);
    void onConnectionError(const ConnectionError& AError);

    void onReceived(const Message2& AMessage) { emit received(AMessage); }
    void onConnected();

    IMessageSender* messageSender() const { return FMessageSender; }

    IFileStreamsManager* getFileManager() const { return FFileManager; }
    IFileTransfer* getFileTransfer() const { return FFileTransfer; }


private:
    void setState(ConnectionState AState) { FState = AState; }
    IFileTransfer *FFileTransfer;
    IFileStreamsManager *FFileManager;
    IMessageSender *FMessageSender;
    ConnectionInfo FInfo;
    ConnectionState FState;
    bool FPinging;
    QString FPingId;
    QString FPingData;
};

#endif // CONNECTIONBASE_H
