#ifndef MESSAGE_H
#define MESSAGE_H

#include "connectioninfo.h"
#include <QByteArray>
#include <utils/stanza.h>
#include <interfaces/istanzaprocessor.h>
#
#define TNOXMPP_PREFIX QString("")
#define TNOXMPP_XMLNS QString("http://rwsoftware.ru/protocols/telnetoverxmpp")

enum ErrorMessageCode {
    EMC_SESSION_ID_IS_NOT_EXISTS = 10000,
    EMC_STANZA_IS_NOT_FOR_ME = 10001,
    EMC_EXCEPTION = 10002
};

enum MessageSenderRole {
    MSR_SERVER,
    MSR_CLIENT,
    MSR_NONE
};

enum MessageType {
    MT_UNKNOWN,
    MT_IQGET,
    MT_IQSET,
    MT_IQERROR,
    MT_IQRESULT,
    MT_MESSAGE
};

class Message2 {
    Stanza *FStanza;
public:

    static bool isValidStanza(const Stanza&);

    QString toString() const {
        return stanza().toString();
    }

    QString id() const;
    Message2& setId(QString AId);

    Message2();
    Message2(const ConnectionInfo &AInfo);
    Message2(const Stanza& AStanza);

    MessageType type() const;
    void setType(MessageType FType);

    QByteArray dataAsByteArray() const;
    QString data() const;
    Message2& setData(const QString& AData);

    QString name() const;
    Message2& setName(const QString& AName);


    QString toSid() const;
    Message2& setToSid(const QString& ASid);

    QString fromSid() const;
    Message2& setFromSid(const QString& ASid);

    Stanza& stanza() { return *FStanza; }
    const Stanza& stanza() const { return *FStanza; }

    Jid from() const { return Jid(stanza().from()); }
    Jid to() const { return Jid(stanza().to()); }

    Message2& setFrom(const Jid& AJid) {
        stanza().setFrom(AJid.full());
        return *this;
    }
    Message2& setTo(const Jid& AJid) {
        stanza().setTo(AJid.full());
        return *this;
    }

    QString attr(const QString& AName) const;
    Message2& setAttr(const QString& AName, const QString& AValue);

    bool isCurrentDirectoryReq() const;

    bool isMessage() const;
    bool isReq() const;
    bool isResult() const;
    bool isSessionOpenReq() const;
    bool isPingReq() const;
    bool isClosedMessage() const;
    bool isDataMessage() const;
    bool isEmpty() const;
    bool isError() const;
    int errorCode() const;
    QString errorMessage() const;
    QDomElement errorElement() const;

    static QString makeNewId();

    static Message2 createEmpty();

    static Message2 createSessionOpenReq(const ConnectionInfo& AInfo);
    static Message2 createSessionCloseReq(const ConnectionInfo& AInfo);
    static Message2 createPingReq(const ConnectionInfo& AInfo, const QString& AData);

    static Message2 createSessionOpenResult(const ConnectionInfo& AInfo, const QString& AId, bool AError=false, int AErrorCode=0, const QString &AErrorMessage="");
    static Message2 createSessionCloseResult(const ConnectionInfo& AInfo, const QString& AId, bool AError=false, int AErrorCode=0, const QString &AErrorMessage="");
    static Message2 createPingResult(const ConnectionInfo& AInfo, const QString& AData, const QString& AId, bool AError=false, int AErrorCode=0, const QString &AErrorMessage="");

    static Message2 createDataMessage(const ConnectionInfo& AInfo, const QString& AData);

    static Message2 createClosedMessage(const ConnectionInfo& AInfo);

    //static Message2 createErrorMessage(const ConnectionInfo& AInfo, int AErrorCode, const QString& AMessage);
private:
    static Message2 createResult(const ConnectionInfo& AInfo, const QString& AId, const QString& AName, bool AError=false, int AErrorCode=0, const QString &AErrorMessage="");
};

struct IMessageSender
{
    virtual void send(const Jid &AJid, const Message2& AMessage) const = 0;
    virtual void request(const Jid &AJid, const Message2& AMessage, int ATimeout) const = 0;
};

class OverStanzaProcessorMessageSender : public IMessageSender
{
public:

    OverStanzaProcessorMessageSender(IStanzaProcessor* AStanzaProcessor,
                                     IStanzaRequestOwner* AStanzaRequestOwner)
        : FStanzaProcessor(AStanzaProcessor),
          FStanzaRequestOwner(AStanzaRequestOwner)
    {
    }

    virtual void send(const Jid &AJid, const Message2& AMessage) const;

    virtual void request(const Jid &AJid, const Message2& AMessage, int ATimeout) const;

    //virtual Message2 makeReplyResult(const Message2 &AMessage) const;


private:
    IStanzaProcessor* FStanzaProcessor;
    IStanzaRequestOwner* FStanzaRequestOwner;
};
#endif // MESSAGE_H
