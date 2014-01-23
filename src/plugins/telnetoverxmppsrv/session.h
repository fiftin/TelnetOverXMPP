#ifndef SESSION_H
#define SESSION_H

#include <QProcess>
#include "../telnetoverxmpp/base/connectionbase.h"
#include <QTextCodec>
#include <interfaces/ifilestreamsmanager.h>

#if defined(Q_OS_WIN32) || defined(Q_OS_WIN64)
#define SESSION_END_OF_COMMAND "\r\n"
#define SESSION_ENCODING_NAME "CP866"
#define SESSION_CURRENT_DIR_COMMAND "cd"
#define SESSION_PATH_SEPARATOR "\\"
#else
#define SESSION_END_OF_COMMAND "\n"
#define SESSION_ENCODING_NAME "UTF-8"
#define SESSION_CURRENT_DIR_COMMAND "pwd"
#define SESSION_PATH_SEPARATOR "/"
#endif

#define SESSION_DOWNLAOD_COMMAND "@"
#define SESSION_DOWNLAOD_BIG_COMMAND "@@"
#define SESSION_RUN_COMMAND "$"
#define SESSION_INVITE_STRING ">"


class FileMessage;

class Session : public ConnectionBase {
    Q_OBJECT

public:
    Session(IMessageSender *AMessageSender,  IFileTransfer *AFileTransfer,
            IFileStreamsManager *AFileManager, const QString& ARemoteSid,
            const Jid& AJid, const Jid& ARemoteJid,
            const QString &AProgram, const QStringList &AArguments=QStringList(),
            QObject *parent = 0)
        : ConnectionBase(AMessageSender, AFileTransfer, AFileManager, makeNewSid(), ARemoteSid, AJid, ARemoteJid, parent),
          FProcess(NULL),
          FProgram(AProgram),
          FArguments(AArguments),
          FTextCodec(QTextCodec::codecForName(SESSION_ENCODING_NAME)),
          FKilled(false),
          FFileMessage(NULL),
          FSendReceivedCommand(true)
    {
        onConnected();
        init();
    }

    virtual ~Session() {
        delete FProcess;
    }

    void init();

    virtual void handleMessage(const Message2&);
    virtual bool fileStreamRequest(int AOrder, const QString &AStreamId, const Stanza &ARequest, const QList<QString> &AMethods);
protected:
    void killProcess();
    virtual void onClosed(ConnectionClosingReason AReason);
    virtual bool handleData(const QString& data);
public slots:
    bool acceptFile();
    void writePWD();
protected slots:
    void onProcessReadyReadStandardOutput();
    void onProcessReadyReadStandardError();
    void onProcessFinished(int exitCode);
    void onStreamStateChanged();
    void onStreamDestroyed();
private:
    QProcess *FProcess;
    QString FProgram;
    QStringList FArguments;
    QTextCodec *FTextCodec;
    bool FKilled;
    FileMessage *FFileMessage;
    bool FSendReceivedCommand;
    QString FStreamId;
    QString FCurrentDirectory;
    QString FLastWrittenCommand;
};

#endif // SESSION_H
