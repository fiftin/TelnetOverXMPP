#include "session.h"
#include "base/filemessage.h"
#include <QFile>
#include <QDir>
void Session::handleMessage(const Message2 &AMessage)
{
    if (FileMessage::isFile(AMessage)) {
        FFileMessage = new FileMessage(AMessage);
        QByteArray dat = FTextCodec->fromUnicode(QString(SESSION_CURRENT_DIR_COMMAND) + SESSION_END_OF_COMMAND);
        FProcess->write(dat);
    }
    else if (AMessage.isDataMessage()) {
        if (!handleData(AMessage.data())) {
            QByteArray dat = FTextCodec->fromUnicode(AMessage.data() + SESSION_END_OF_COMMAND);
            if (FSendReceivedCommand) {
                send(QString(SESSION_INVITE_STRING) + " " + AMessage.data() + "\n");
            }
            FProcess->write(dat);
        }
    }
    ConnectionBase::handleMessage(AMessage);
}

bool Session::handleData(const QString &data)
{
    // Download file command received: @ <file_name>
    if (data.startsWith(QString(SESSION_DOWNLAOD_COMMAND) + " ", Qt::CaseInsensitive)) {
        QString filename = data.mid(strlen(SESSION_DOWNLAOD_COMMAND) + 1);
        if (!sendFile(filename)) {
            send("\nDo not sent a file '"+filename+"'\n");
        }
        return true;
    }
    // Download file command received: @@ <file_name>
    else if (data.startsWith(QString(SESSION_DOWNLAOD_BIG_COMMAND) + " ", Qt::CaseInsensitive)) {
        QString filename = data.mid(strlen(SESSION_DOWNLAOD_BIG_COMMAND) + 1);
        if (!sendBigFile(filename)) {
            send("\nDo not sent a file '"+filename+"'\n");
        }
        return true;
    }
    // Run other program - NOT WORKS
    else if (data.startsWith(QString(SESSION_RUN_COMMAND) + " ", Qt::CaseInsensitive)) {
        FKilled = true;
        FProcess->close();
        FProgram = data.mid(strlen(SESSION_RUN_COMMAND) + 1);
        FArguments.clear();
        init();
        return true;
    }
    return false;
}

void Session::killProcess()
{
    FKilled = true;
    if (FProcess->state() != QProcess::NotRunning) {
        FProcess->close();
    }
}

void Session::onClosed(ConnectionClosingReason AReason)
{
    killProcess();
    ConnectionBase::onClosed(AReason);
}

void Session::onProcessReadyReadStandardOutput()
{
    QByteArray dat = FProcess->readAllStandardOutput();
    QString result = FTextCodec->toUnicode(dat);
    if (FFileMessage != NULL) {
        QString currentDirectory = "";
        QStringList lines = result.split('\n');
        foreach (QString l, lines) {
            QString trimmed = l.trimmed();
            if (trimmed != "" && trimmed != SESSION_CURRENT_DIR_COMMAND) {
                currentDirectory = trimmed;
                break;
            }
        }
        if (currentDirectory != "") {
            if (FFileMessage->getMethod() == FMM_INTERNAL) {
                QFile *file = new QFile(currentDirectory + QDir::separator() + FFileMessage->fileName());
                file->open(QFile::WriteOnly);
                file->write(FFileMessage->internalContent());
                file->close();
                send("File '" + FFileMessage->fileName() + "' saved on remote PC.\n");
                //delete file;
            }
            else if (FFileMessage->getMethod() == FMM_FILETRANSFER) {
                QString streamId = FFileMessage->internalContentStr();
                IFileStream *stream = getFileManager()->streamById(streamId);

            }
        }
        else {
            send("File '" + FFileMessage->fileName() + "' DO NOT saved on remote PC.\n");
        }
        delete FFileMessage;
        FFileMessage = NULL;
    }
    else {
        send(result);
    }
}

void Session::onProcessReadyReadStandardError()
{
    QByteArray dat = FProcess->readAllStandardError();
    QString error = FTextCodec->toUnicode(dat);
    send(error);
}

void Session::onProcessFinished(int exitCode)
{
    Q_UNUSED(exitCode);
    if (!FKilled)
        close();
}

void Session::init()
{
    FProcess = new QProcess();
    this->connect(FProcess, SIGNAL(readyReadStandardOutput()), SLOT(onProcessReadyReadStandardOutput()));
    this->connect(FProcess, SIGNAL(readyReadStandardError()), SLOT(onProcessReadyReadStandardError()));
    this->connect(FProcess, SIGNAL(finished(int)), SLOT(onProcessFinished(int)));
    FProcess->start(FProgram, FArguments);
}
