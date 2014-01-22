#ifndef CONNECTIONINFO_H
#define CONNECTIONINFO_H

#include <QString>
#include <utils/jid.h>
struct ConnectionInfo
{
    ConnectionInfo() {

    }

    ConnectionInfo(QString ASid, QString ARemoteSid, const Jid& AJid, const Jid& ARemoteJid)
        : sid(ASid),
          remoteSid(ARemoteSid),
          jid(AJid),
          remoteJid(ARemoteJid)
    {
    }

    QString sid;
    QString remoteSid;
    Jid jid;
    Jid remoteJid;
};

#endif // CONNECTIONINFO_H
