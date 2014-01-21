#ifndef TELNETOVERXMPPCLIENT_H
#define TELNETOVERXMPPCLIENT_H

#include <QObject>

#include <interfaces/ipluginmanager.h>
#include <interfaces/iservicediscovery.h>
#include <interfaces/irostersview.h>
#include <interfaces/istanzaprocessor.h>
#include <interfaces/iservicediscovery.h>
#include <itelnetoverxmppclient.h>
#include <interfaces/ifilestreamsmanager.h>
#include <interfaces/ifiletransfer.h>

#include "remoteterminaldialog.h"
#include "base/telnetoverxmppbase.h"

class TelnetOverXmppClient : public QObject, public IPlugin,
        public ITelnetOverXmppClient, public IStanzaHandler,
        public IStanzaRequestOwner, public TelnetOverXmppBase
{
    Q_OBJECT
    Q_INTERFACES(IPlugin ITelnetOverXmppClient IStanzaHandler IStanzaRequestOwner)


public:
    explicit TelnetOverXmppClient(QObject *parent = 0);
    virtual ~TelnetOverXmppClient() { }
    virtual QObject *instance() { return this; }
    virtual QUuid pluginUuid() const { return QUuid("{8642b439-cac5-440a-9b31-7d39f4b7960d}"); }
    virtual void pluginInfo(IPluginInfo *APluginInfo);
    virtual bool initConnections(IPluginManager *APluginManager, int &AInitOrder);
    virtual bool initObjects();
    virtual bool initSettings();
    virtual bool startPlugin();
    virtual bool stanzaReadWrite(int AHandleId, const Jid &AJid, Stanza &AStanza, bool &AAccept);
    virtual void stanzaRequestResult(const Jid &AJid, const Stanza &AStanza);

public slots:

    void onRosterIndexContextMenu(const QList<IRosterIndex *> &AIndexes, quint32 ALabelId, Menu *AMenu);
    void onConnectToTerminalByAction(bool);
private:
    RemoteTerminalDialog *FTerminalDialog;
    IStanzaProcessor *FStanzaProcessor;
    IServiceDiscovery *FServiceDiscovery;
    IRostersViewPlugin *FRostersViewPlugin;
    IMessageSender *FMessageSender;
    IFileStreamsManager *FFileManager;
    IFileTransfer *FFileTransfer;
};

#endif // TELNETOVERXMPPCLIENT_H
