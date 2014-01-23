#ifndef TELNETOVERXMPPSRV_H
#define TELNETOVERXMPPSRV_H

#include <QObject>

#include <interfaces/ipluginmanager.h>
#include <interfaces/iservicediscovery.h>
#include <interfaces/irostersview.h>
#include <interfaces/istanzaprocessor.h>
#include <interfaces/ifilestreamsmanager.h>
#include <interfaces/ifiletransfer.h>
#include <interfaces/imainwindow.h>
#include <interfaces/itraymanager.h>

#include "itelnetoverxmppsrv.h"
#include "../telnetoverxmpp/base/telnetoverxmppbase.h"
#include "session.h"
#include "sessionsdialog.h"

#if defined(Q_OS_WIN32) || defined(Q_OS_WIN64)
#define TERMINAL_PROGRAM_NAME "cmd.exe"
#else
#define TERMINAL_PROGRAM_NAME "dash"
#endif

class TelnetOverXmppSrv : public QObject, public IPlugin, public ITelnetOverXmppSrv,
        public IStanzaHandler, public TelnetOverXmppBase
{
    Q_OBJECT
    Q_INTERFACES(IPlugin ITelnetOverXmppSrv IStanzaHandler)
public:
    explicit TelnetOverXmppSrv(QObject *parent = 0);
    virtual QObject *instance() { return this; }
    virtual QUuid pluginUuid() const { return QUuid("{b3896270-9c8d-44e1-b531-f938ad855157}"); }
    virtual void pluginInfo(IPluginInfo *APluginInfo);
    virtual bool initConnections(IPluginManager *APluginManager, int &AInitOrder);
    virtual bool initObjects();
    virtual bool initSettings();
    virtual bool startPlugin();
    virtual bool stanzaReadWrite(int AHandleId, const Jid &AJid, Stanza &AStanza, bool &AAccept);
public slots:
    void onSessionClosed(ConnectionClosingReason);
protected slots:
    void onShowTNOXMPPSessionsWindow(bool);

private:
    IServiceDiscovery *FServiceDiscovery;
    IStanzaProcessor *FStanzaProcessor;
    IMessageSender *FMessageSender;
    IFileStreamsManager *FFileManager;
    IFileTransfer *FFileTransfer;
    IMainWindowPlugin *FMainWindowPlugin;
    ITrayManager *FTrayManager;
    SessionsDialog *FSessionsDialog;
};

#endif // TELNETOVERXMPPSRV_H
