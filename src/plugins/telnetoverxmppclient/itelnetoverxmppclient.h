#ifndef ITELNETOVERXMPPCLIENT_H
#define ITELNETOVERXMPPCLIENT_H
class QObject;
class ITelnetOverXmppClient
{
public:
    ITelnetOverXmppClient() { }
    virtual QObject *instance() =0;
};
Q_DECLARE_INTERFACE(ITelnetOverXmppClient, "FiftiN.Plugin.ITelnetOverXmppClient/1.0")
#endif // ITELNETOVERXMPPCLIENT_H
