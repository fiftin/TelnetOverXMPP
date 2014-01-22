#include "message2.h"
#include <QDebug>
#include "exception.h"
#include <QUuid>

#define IQ QString("iq")
#define IQ_GET QString("get")
#define IQ_SET QString("set")
#define IQ_RESULT QString("result")
#define IQ_ERROR QString("error")
#define MSG QString("message")

#define TNOXMPP_DATA QString("data")
#define TNOXMPP_PING QString("ping")
#define TNOXMPP_PINGREPLY QString("pong")
#define TNOXMPP_CONNECT QString("connect")
#define TNOXMPP_CONNECTED QString("connected")
#define TNOXMPP_DISCONNECT QString("disconnect")
#define TNOXMPP_DISCONNECTED QString("disconnected")
#define TNOXMPP_TERMINATED QString("terminated")
#define TNOXMPP_ERROR QString("error")

Message2::Message2()
{
    FStanza = new Stanza();
    QDomElement el = stanza().addElement(TNOXMPP_DATA, TNOXMPP_XMLNS);
    QDomText dataSec = stanza().document().createTextNode("");
    el.appendChild(dataSec);
}

Message2::Message2(const ConnectionInfo &AInfo)
{
    FStanza = new Stanza();
    QDomElement el = stanza().addElement(TNOXMPP_DATA, TNOXMPP_XMLNS);
    QDomText dataSec = stanza().document().createTextNode("");
    el.appendChild(dataSec);
    setFrom(AInfo.jid);
    setTo(AInfo.remoteJid);
    setFromSid(AInfo.sid);
    setToSid(AInfo.remoteSid);
}

Message2::Message2(const Stanza& AStanza)
    : FStanza(new Stanza(AStanza.element().cloneNode().toElement()))
{
}

MessageType Message2::type() const {
    if (stanza().tagName() == MSG)
        return MT_MESSAGE;
    else if (stanza().tagName() == IQ) {
        if (stanza().type() == IQ_GET)
            return MT_IQGET;
        else if (stanza().type() == IQ_SET)
            return MT_IQSET;
        else if (stanza().type() == IQ_RESULT)
            return MT_IQRESULT;
        else if (stanza().type() == IQ_ERROR)
            return MT_IQERROR;
    }
    return MT_UNKNOWN;
}

void Message2::setType(MessageType FType) {
    switch (FType) {
    case MT_IQGET:
        stanza().setType(IQ_GET);
        stanza().setTagName(IQ);
        break;
    case MT_IQSET:
        stanza().setType(IQ_SET);
        stanza().setTagName(IQ);
        break;
    case MT_IQRESULT:
        stanza().setType(IQ_RESULT);
        stanza().setTagName(IQ);
        break;
    case MT_IQERROR:
        stanza().setType(IQ_ERROR);
        stanza().setTagName(IQ);
        break;
    case MT_MESSAGE:
        stanza().setTagName(MSG);
        break;
    case MT_UNKNOWN:
        break;
    }
}

QByteArray Message2::dataAsByteArray() const
{
    return data().toAscii();
}

QString Message2::id() const
{
    return stanza().id();
}

Message2& Message2::setId(QString AId)
{
    stanza().setId(AId);
    return *this;
}

QString Message2::data() const
{
    if (isEmpty())
        return QString();
    return stanza().firstElement().firstChild().toText().data();
}

Message2& Message2::setData(const QString &AData)
{
    stanza().firstElement().firstChild().toText().setData(AData);
    return *this;
}

QString Message2::name() const
{
    if (isEmpty())
        return QString();
    return stanza().firstElement().tagName();
}

Message2& Message2::setName(const QString &AName)
{
    stanza().firstElement().setTagName(AName);
    return *this;
}

bool Message2::isValidStanza(const Stanza &AStanza)
{
    return true;
}

QString Message2::toSid() const
{
    if (isEmpty())
        return QString();
    QDomElement elem = stanza().firstElement();
    return elem.attribute("to-sid", "");
}

Message2& Message2::setToSid(const QString &ASid)
{
    stanza().firstElement().setAttribute("to-sid", ASid);
    return *this;
}

QString Message2::fromSid() const
{
    if (isEmpty())
        return QString();
    return stanza().firstElement().attribute("from-sid", "");
}

Message2& Message2::setFromSid(const QString &ASid)
{
    stanza().firstElement().setAttribute("from-sid", ASid);
    return *this;
}

QString Message2::attr(const QString &AName) const
{
    if (isEmpty())
        return QString();
    return stanza().firstElement().attribute(AName);
}

Message2& Message2::setAttr(const QString &AName, const QString &AValue)
{
    stanza().firstElement().setAttribute(AName, AValue);
    return *this;
}

bool Message2::isCurrentDirectoryReq() const
{
    return false;
}

bool Message2::isMessage() const
{
    return stanza().tagName() == MSG;
}

bool Message2::isReq() const {
    if (isEmpty())
        return false;
    return stanza().tagName() == IQ
            && (stanza().type() == IQ_GET ||
                stanza().type() == IQ_SET);
}


bool Message2::isResult() const {
    if (isEmpty())
        return false;
    return stanza().tagName() == IQ
            && (stanza().type() == IQ_RESULT ||
                stanza().type() == IQ_ERROR);
}


bool Message2::isSessionOpenReq() const {
    if (isEmpty())
        return false;
    return isReq() && name() == TNOXMPP_CONNECT;
}

bool Message2::isPingReq() const {
    if (isEmpty())
        return false;
    return isReq() && name() == TNOXMPP_PING;
}

bool Message2::isClosedMessage() const {
    if (isEmpty())
        return false;
    return isMessage() && name() == TNOXMPP_TERMINATED;
}

bool Message2::isDataMessage() const {
    return name() == TNOXMPP_DATA;
}

bool Message2::isError() const
{
    if (type() == MT_IQERROR)
        return true;
    else if (isEmpty())
        return false;

    bool hasErrorEl = false;
    QDomNodeList children = stanza().element().childNodes();
    for (int i = 0; i < children.size(); i++) {
        if (children.item(i).isElement()) {
            QDomElement el = children.item(i).toElement();
            if (el.tagName() == TNOXMPP_ERROR) {
                hasErrorEl = true;
                break;
            }
        }
    }
    return hasErrorEl;
}

int Message2::errorCode() const
{
    if (!isError() || isEmpty())
        return 0;
    return attr("code").toInt();
}

QString Message2::errorMessage() const
{
    if (!isError())
        return QString();
    QDomElement el = errorElement();
    if (el.isNull())
        return QString();
    if (el.firstChild().isText())
        return el.firstChild().toText().data();
    else if (el.firstChild().isElement())
        return el.firstChild().toElement().text();
    else
        return "";
}

QDomElement Message2::errorElement() const
{
    QDomNodeList children = stanza().element().childNodes();
    for (int i = 0; i < children.size(); i++) {
        if (children.item(i).isElement()) {
            QDomElement el = children.item(i).toElement();
            if (el.tagName() == TNOXMPP_ERROR)
                return el;
        }
    }
    return QDomNode().toElement();
}

bool Message2::isEmpty() const
{
    const QDomDocument& doc = stanza().document();
    const QDomElement& elem = doc.documentElement();
    return elem.childNodes().size() == 0;
}

QString Message2::makeNewId()
{
    QString uuid = QUuid::createUuid().toString().mid(25, 12);
    return uuid;
}

Message2 Message2::createEmpty()
{
    return Message2(Stanza());
}

Message2 Message2::createDataMessage(const ConnectionInfo& AInfo, const QString &AData)
{
    Message2 result(AInfo);
    result.setName(TNOXMPP_DATA);
    result.setData(AData);
    return result;
}

Message2 Message2::createSessionOpenReq(const ConnectionInfo& AInfo)
{
    Message2 result(AInfo);
    result.setType(MT_IQSET);
    QString id = makeNewId();
    result.setId(id);
    result.setName(TNOXMPP_CONNECT);
    return result;
}

Message2 Message2::createSessionCloseReq(const ConnectionInfo &AInfo)
{
    Message2 result(AInfo);
    result.setType(MT_IQSET);
    result.setName(TNOXMPP_DISCONNECT);
    result.setId(makeNewId());
    return result;
}

Message2 Message2::createPingReq(const ConnectionInfo& AInfo, const QString& AData)
{
    Message2 result(AInfo);
    result.setType(MT_IQGET);
    result.setName(TNOXMPP_PING);
    result.setId(makeNewId());
    result.setData(AData);
    return result;
}

Message2 Message2::createResult(const ConnectionInfo &AInfo, const QString &AId, const QString& AName, bool AError, int AErrorCode, const QString &AErrorMessage)
{
    Message2 result(AInfo);
    if (AError) {
        result.setName(TNOXMPP_ERROR);
        result.setAttr("code", QString::number(AErrorCode));
        result.setData(AErrorMessage);
        result.setType(MT_IQERROR);
    }
    else {
        result.setName(AName);
        result.setType(MT_IQRESULT);
    }
    result.setId(AId);
    return result;
}

Message2 Message2::createSessionCloseResult(const ConnectionInfo &AInfo, const QString &AId, bool AError, int AErrorCode, const QString &AErrorMessage)
{
    return createResult(AInfo, AId, TNOXMPP_DISCONNECTED, AError, AErrorCode, AErrorMessage);
}

Message2 Message2::createSessionOpenResult(const ConnectionInfo& AInfo, const QString& AId, bool AError, int AErrorCode, const QString &AErrorMessage)
{
    return createResult(AInfo, AId, TNOXMPP_CONNECTED, AError, AErrorCode, AErrorMessage);
}

Message2 Message2::createPingResult(const ConnectionInfo& AInfo, const QString& AData, const QString& AId, bool AError, int AErrorCode, const QString &AErrorMessage)
{
    Message2 result(createResult(AInfo, AId, TNOXMPP_PINGREPLY, AError, AErrorCode, AErrorMessage));
    if (!AError)
        result.setData(AData);
    return result;
}

Message2 Message2::createClosedMessage(const ConnectionInfo& AInfo)
{
    return Message2(AInfo).setName(TNOXMPP_TERMINATED);
}

/*
Message2 Message2::createErrorMessage(const ConnectionInfo &AInfo, int AErrorCode, const QString &AErrorMessage)
{
    Message2 result(AInfo);
    result.setName(TNOXMPP_ERROR);
    result.setAttr("code", QString::number(AErrorCode));
    result.setData(AErrorMessage);
    return result;
}
 */

////////////////////////////////////////////////////////////////////////

void OverStanzaProcessorMessageSender::send(const Jid &AJid, const Message2& AMessage) const {
    Stanza stanza = AMessage.stanza();
    try {
        FStanzaProcessor->sendStanzaOut(AJid, stanza);
    }
    catch (...) {
    }

}

void OverStanzaProcessorMessageSender::request(const Jid &AJid, const Message2& AMessage, int ATimeout) const {
    Stanza stanza = AMessage.stanza();
    FStanzaProcessor->sendStanzaRequest(FStanzaRequestOwner, AJid, stanza, ATimeout);
}
