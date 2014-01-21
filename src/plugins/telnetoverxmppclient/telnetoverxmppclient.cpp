#include "telnetoverxmppclient.h"
#include <definitions/rosterindexkinds.h>
#include <definitions/rosterindexroles.h>
#include <QDebug>
#include <definitions/resources.h>
#define ADR_STREAM_JID 1
#define ADR_REMOTE_JID 2

TelnetOverXmppClient::TelnetOverXmppClient(QObject *parent) :
    QObject(parent),
    FTerminalDialog(NULL),
    FStanzaProcessor(NULL),
    FServiceDiscovery(NULL),
    FRostersViewPlugin(NULL),
    FMessageSender(NULL)
{
}

void TelnetOverXmppClient::pluginInfo(IPluginInfo *APluginInfo) {
    APluginInfo->author = "FiftiN";
    APluginInfo->name = "TelnetOverXmppClient";
    APluginInfo->version = "1.0";
    APluginInfo->description = tr("Access to remote terminal over XMPP");
    APluginInfo->homePage = "http://rwsoftware.ru/protocols/telnetoverxmpp";

}

bool TelnetOverXmppClient::initConnections(IPluginManager *APluginManager, int &AInitOrder) {

    IPlugin* plugin = APluginManager->pluginInterface("IServiceDiscovery").value(0, NULL);
    if (plugin != NULL) {
        FServiceDiscovery = qobject_cast<IServiceDiscovery*>(plugin->instance());
    }

    plugin = APluginManager->pluginInterface("IRostersViewPlugin").value(0, NULL);
    if (plugin != NULL) {
        FRostersViewPlugin = qobject_cast<IRostersViewPlugin *>(plugin->instance());
        connect(FRostersViewPlugin->rostersView()->instance(),

                SIGNAL(indexContextMenu(QList<IRosterIndex *>, quint32, Menu *)),
                SLOT(onRosterIndexContextMenu(QList<IRosterIndex *>, quint32, Menu *)));
    }

    plugin = APluginManager->pluginInterface("IStanzaProcessor").value(0, NULL);
    if (plugin != NULL) {
        FStanzaProcessor = qobject_cast<IStanzaProcessor *>(plugin->instance());
        FStanzaProcessor->insertStanzaHandle(makeStanzaHandle(this));
        FMessageSender = new OverStanzaProcessorMessageSender(FStanzaProcessor, this);
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

bool TelnetOverXmppClient::initObjects() {
    return true;
}

bool TelnetOverXmppClient::initSettings() {
    return true;
}

bool TelnetOverXmppClient::startPlugin() {
    return true;
}

bool TelnetOverXmppClient::stanzaReadWrite(int AHandleId, const Jid &AJid, Stanza &AStanza, bool &AAccept) {
    if (Jid(AStanza.from()).bare() != AJid.bare())
        return false;
    if (!Message2::isValidStanza(AStanza))
        return false;
    Message2 message(AStanza);
    bool ret = tryHandleMessage(message);
    return ret;
}

void TelnetOverXmppClient::stanzaRequestResult(const Jid &AJid, const Stanza &AStanza)
{
    Message2 message(AStanza);
    if (message.isResult()) {
        QList<ConnectionBase*> closedConnections = removeClosedConnections();
        foreach (ConnectionBase* c, closedConnections) {
            delete c;
        }
        Connection *connection = qobject_cast<Connection*>(getConnection(message.toSid()));
        if (connection != NULL
                && connection->jid() == AJid
                && connection->isMessageForMe(message)) {
            connection->handleMessage(message);
        }
        else if (message.id() != "") {
            foreach (ConnectionBase* c, getConnections()) {
                Connection *connection = qobject_cast<Connection*>(c);
                if (connection->connectionRequestId() == message.id()) {
                    connection->handleMessage(message);
                    break;
                }
            }
        }
    }
}

void TelnetOverXmppClient::onRosterIndexContextMenu(const QList<IRosterIndex *> &AIndexes, quint32 ALabelId, Menu *AMenu)
{
    if (AIndexes.length() == 1) {
        IRosterIndex *index = AIndexes.value(0);
        if (index->kind() == RIK_CONTACT || index->kind() == RIK_MY_RESOURCE) {
            Jid contactJid(index->data(RDR_FULL_JID).toString());
            Jid streamJid(index->data(RDR_STREAM_JID).toString());
            QStringList features = FServiceDiscovery->discoInfo(streamJid, contactJid).features;
            if (contactJid.bare() == streamJid.bare()
                    && features.contains(TNOXMPP_XMLNS)
                    ) {
                Action *action = new Action(AMenu);
                action->setText(tr("Remote terminal"));
                action->setData(ADR_STREAM_JID, streamJid.full());
                action->setData(ADR_REMOTE_JID, contactJid.full());
                action->setIcon(RSR_STORAGE_MENUICONS, "telnetoverxmppicon");
                AMenu->addAction(action, AG_DEFAULT, true);
                connect(action, SIGNAL(triggered(bool)), SLOT(onConnectToTerminalByAction(bool)));
            }
        }
    }
}

void TelnetOverXmppClient::onConnectToTerminalByAction(bool)
{
    if (FTerminalDialog == NULL)
        FTerminalDialog = new RemoteTerminalDialog();
    Action *action = qobject_cast<Action *>(sender());
    Connection *connection = new Connection(FMessageSender,
                                            action->data(ADR_STREAM_JID).toString(),
                                            action->data(ADR_REMOTE_JID).toString(), false);
    FTerminalDialog->addTerminal(connection);
    FTerminalDialog->showDialog();
    addConnection(connection);
}

Q_EXPORT_PLUGIN2(plg_telnetoverxmppclient, TelnetOverXmppClient)
