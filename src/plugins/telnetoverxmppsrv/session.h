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
#define SESSION_INVITE_STRING ">"
#define SESSION_PWD_PATTERN ""
#define SESSION_SEND_RECEIVED_COMMAND false
#define SESSION_CASE_SESATIVITY false
#define SESSION_DEFAULT_WORKING_DIRECTORY "C:\\"
#else
#define SESSION_END_OF_COMMAND "\n"
#define SESSION_ENCODING_NAME "UTF-8"
#define SESSION_CURRENT_DIR_COMMAND "pwd"
#define SESSION_PATH_SEPARATOR "/"
#define SESSION_INVITE_STRING "$"
#define SESSION_CASE_SESATIVITY true
#define SESSION_SEND_RECEIVED_COMMAND true
#define SESSION_DEFAULT_WORKING_DIRECTORY "/"
#endif

#define SESSION_CD_COMMAND "cd"
#define SESSION_CURRENT_DIR_COMMAND_INTERNAL "__pwd__3820437_37781332_e73s36www7273932"
#define SESSION_DOWNLAOD_COMMAND "!"
#define SESSION_DOWNLAOD_BIG_COMMAND "@"

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
          FSendReceivedCommand(SESSION_SEND_RECEIVED_COMMAND)
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
    void onStarted();
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
