#include "telnetoverxmppsrv.h"
#include "../telnetoverxmpp/base/exception.h"

#include <definitions/resources.h>
TelnetOverXmppSrv::TelnetOverXmppSrv(QObject *parent) :
    QObject(parent)
{
}

void TelnetOverXmppSrv::pluginInfo(IPluginInfo *APluginInfo) {
    APluginInfo->author = "FiftiN";
    APluginInfo->name = "TelnetOverXmppSrv";
    APluginInfo->version = "1.0";
    APluginInfo->description = tr("Access from remote client over XMPP");
    APluginInfo->homePage = "http://rwsoftware.ru/protocols/telnetoverxmpp";
}

bool TelnetOverXmppSrv::initConnections(IPluginManager *APluginManager, int &AInitOrder) {


    IPlugin* plugin = APluginManager->pluginInterface("IServiceDiscovery").value(0, NULL);
    if (plugin != NULL) {
        FServiceDiscovery = qobject_cast<IServiceDiscovery*>(plugin->instance());
    }

    plugin = APluginManager->pluginInterface("IStanzaProcessor").value(0, NULL);
    if (plugin != NULL) {
        FStanzaProcessor = qobject_cast<IStanzaProcessor *>(plugin->instance());
        FStanzaProcessor->insertStanzaHandle(makeStanzaHandle(this));
        FMessageSender = new OverStanzaProcessorMessageSender(FStanzaProcessor, NULL);
    }

    plugin = APluginManager->pluginInterface("IFileStreamsManager").value(0,NULL);
    if (plugin) {
        FFileManager = qobject_cast<IFileStreamsManager *>(plugin->instance());
    }

    plugin = APluginManager->pluginInterface("IFileTransfer").value(0,NULL);
    if (plugin) {
        FFileTransfer = qobject_cast<IFileTransfer *>(plugin->instance());
    }

    return true;
}

bool TelnetOverXmppSrv::initObjects() {
    IDiscoFeature feature;
    feature.description = tr("Access from remote client over XMPP");
    feature.name = tr("Telnet over XMPP");
    feature.var = TNOXMPP_XMLNS;
    feature.active = true;
    feature.icon = IconStorage::staticStorage(RSR_STORAGE_MENUICONS)->getIcon("telnetoverxmpp");
    FServiceDiscovery->insertDiscoFeature(feature);
    return true;
}

bool TelnetOverXmppSrv::initSettings() {
    return true;
}

bool TelnetOverXmppSrv::startPlugin() {
    return true;
}

bool TelnetOverXmppSrv::stanzaReadWrite(int AHandleId, const Jid &AJid, Stanza &AStanza, bool &AAccept) {
    Q_UNUSED(AHandleId)
    Q_UNUSED(AAccept)
    try {
        if (Jid(AStanza.from()).bare() != AJid.bare())
            return false;

        if (!Message2::isValidStanza(AStanza))
            return false;

        Message2 message(AStanza);

        if (message.isResult())
            return false;

        if (message.isSessionOpenReq()) {
            Session* session = qobject_cast<Session*>(getConnection(message.toSid()));
            if (session != NULL) {
                Message2 reply = Message2::createSessionOpenResult(session->info(), message.id());
                FMessageSender->send(AJid, reply);
            }
            else {
                try {

                    QList<ConnectionBase*> closedConnections = removeClosedConnections();
                    foreach (ConnectionBase* c, closedConnections) {
                        delete c;
                    }
                    session = new Session(FMessageSender, FFileTransfer, FFileManager, message.fromSid(), AJid, message.from(), TERMINAL_PROGRAM_NAME);
                    addConnection(session);
                    connect(session,
                            SIGNAL(closed(ConnectionClosingReason)),
                            SLOT(onSessionClosed(ConnectionClosingReason)));
                    Message2 reply = Message2::createSessionOpenResult(session->info(), message.id());
                    FMessageSender->send(AJid, reply);
                }
                catch (Exception ex) {
                    if (session != NULL) {
                        if (getConnection(session->sid()) != NULL) {
                            removeConnection(session);
                        }
                        delete session;
                    }
                    Message2 msg = Message2::createSessionOpenResult(ConnectionInfo("", message.fromSid(), AJid, message.from()), message.id(), true);
                    msg.setData(ex.message());
                    FMessageSender->send(AJid, msg);
                }
            }
        }
        else {
            bool ret = tryHandleMessage(message);
            return ret;
        }
        return true;
    }
    catch (Exception *) {
        return false;
    }
    catch (...) {
        return false;
    }
}



void TelnetOverXmppSrv::onSessionClosed(ConnectionClosingReason)
{
    ConnectionBase* session = qobject_cast<ConnectionBase*>(sender());
    removeConnection(session);
}


Q_EXPORT_PLUGIN2(plg_telnetoverxmppsrv, TelnetOverXmppSrv)
