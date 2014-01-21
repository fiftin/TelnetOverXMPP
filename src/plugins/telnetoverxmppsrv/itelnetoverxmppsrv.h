#ifndef ITELNETOVERXMPPSRV_H
#define ITELNETOVERXMPPSRV_H
class QObject;
class ITelnetOverXmppSrv
{
public:
    ITelnetOverXmppSrv() { }
    virtual QObject *instance() =0;
};
Q_DECLARE_INTERFACE(ITelnetOverXmppSrv, "FiftiN.Plugin.ITelnetOverXmppSrv/1.0")

#endif // ITELNETOVERXMPPSRV_H
