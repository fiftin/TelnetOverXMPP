#include "session.h"
#include "../telnetoverxmpp/base/filemessage.h"
#include <QFile>
#include <QDir>
#include <QTimer>
#include <interfaces/ifiletransfer.h>
#include <interfaces/ifilestreamsmanager.h>
#include <definitions/optionvalues.h>
#include "../telnetoverxmpp/base/controlcmessage.h"
#include "../telnetoverxmpp/base/pwdmessage.h"
#include "../telnetoverxmpp/base/commandmessage.h"

void Session::handleMessage(const Message2 &AMessage)
{
    if (FileMessage::isFile(AMessage)) {
        FFileMessage = new FileMessage(AMessage);
        writePWD();
    }
    else if (ControlCMessage::isControlCMessage(AMessage)) {
        FProcess->kill();
    }
    else if (AMessage.isDataMessage()) {
        if (!handleData(AMessage.data())) {
            QByteArray dat = FTextCodec->fromUnicode(AMessage.data() + SESSION_END_OF_COMMAND);
            if (FSendReceivedCommand) {
                QString prompt = SESSION_INVITE_STRING;
                if (!prompt.isEmpty())
                    prompt = FCurrentDirectory + prompt;
                send(prompt + AMessage.data() + "\n");
            }
            FLastWrittenCommand = AMessage.data();
            FProcess->write(dat);
            QTimer::singleShot(300, this, SLOT(writePWD()));
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
        if (!sendBigFile(filename, true)) {
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
    if (FLastWrittenCommand == SESSION_CURRENT_DIR_COMMAND_INTERNAL) {
        FCurrentDirectory = "";
        QStringList lines = result.split('\n');
        foreach (QString l, lines) {
            QString trimmed = l.trimmed();
            if (trimmed != "" && trimmed != SESSION_CURRENT_DIR_COMMAND) {
                FCurrentDirectory = trimmed;
                break;
            }
        }
        send(PWDMessage::createMessage(info(), FCurrentDirectory + SESSION_INVITE_STRING));
    }
    if (FFileMessage != NULL) {

        if (FCurrentDirectory != "") {
            if (FFileMessage->getMethod() == FMM_INTERNAL) {
                QFile *file = new QFile(FCurrentDirectory + QDir::separator() + FFileMessage->fileName());
                file->open(QFile::WriteOnly);
                file->write(FFileMessage->internalContent());
                file->close();
                send("File '" + FFileMessage->fileName() + "' saved on remote PC.\n");
                FCurrentDirectory = "";
            }
            else if (FFileMessage->getMethod() == FMM_FILETRANSFER) {
                FStreamId = FFileMessage->internalContentStr();
                if (!acceptFile())
                    getFileManager()->insertStreamsHandler(this, 0);
            }
        }
        else {
            send("File '" + FFileMessage->fileName() + "' <b>DO NOT</b> saved on remote PC.\n");
        }
        delete FFileMessage;
        FFileMessage = NULL;
    }
    else if (FLastWrittenCommand != SESSION_CURRENT_DIR_COMMAND_INTERNAL) {
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

void Session::writePWD()
{
    QByteArray dat = FTextCodec->fromUnicode(QString(SESSION_CURRENT_DIR_COMMAND) + SESSION_END_OF_COMMAND);
    FLastWrittenCommand = SESSION_CURRENT_DIR_COMMAND_INTERNAL;
    FProcess->write(dat);
}

void Session::init()
{
    FProcess = new QProcess();

    this->connect(FProcess, SIGNAL(started()), SLOT(onStarted()));
    this->connect(FProcess, SIGNAL(readyReadStandardOutput()), SLOT(onProcessReadyReadStandardOutput()));
    this->connect(FProcess, SIGNAL(readyReadStandardError()), SLOT(onProcessReadyReadStandardError()));
    this->connect(FProcess, SIGNAL(finished(int)), SLOT(onProcessFinished(int)));
    FProcess->start(FProgram, FArguments);
}

bool Session::fileStreamRequest(int AOrder, const QString &AStreamId, const Stanza &ARequest, const QList<QString> &AMethods)
{
    if (AStreamId == FStreamId)
        QTimer::singleShot(1000, this, SLOT(acceptFile()));
    return false;
}

bool Session::acceptFile()
{
    IFileStream *stream = getFileManager()->streamById(FStreamId);
    if (stream == NULL)
        return false;

    connect(stream->instance(),SIGNAL(stateChanged()),SLOT(onStreamStateChanged()));
    connect(stream->instance(),SIGNAL(streamDestroyed()),SLOT(onStreamDestroyed()));

    QString defaultMethod = Options::node(OPV_FILESTREAMS_DEFAULTMETHOD).value().toString();
    bool ret = false;
    if (stream->acceptableMethods().contains(defaultMethod))
        ret = stream->startStream(defaultMethod);
    else if (!stream->acceptableMethods().isEmpty())
        ret = stream->startStream(stream->acceptableMethods().at(0));
    if (ret)
        getFileManager()->removeStreamsHandler(this, 0);
    return ret;
}

void Session::onStreamStateChanged()
{
    IFileStream *stream = qobject_cast<IFileStream *>(sender());
    if (stream)
    {
        if (stream->streamState() == IFileStream::Finished)
        {
            QFile file(stream->fileName());
            QString fileName = QFileInfo(file).fileName();
            if (file.exists()) {
                QString fileNewPath = FCurrentDirectory + QDir::separator() + fileName;
                if (file.rename(fileNewPath))
                    send("File '" + fileName + "' saved on remote PC.\n");
                else
                    send("File '" + fileName + "' <b>DO NOT</b> saved on remote PC.\n");
            }
            else {
                send("File '" + fileName + "' <b>DO NOT</b> saved on remote PC.\n");

            }
            FCurrentDirectory = "";
        }
    }
}

void Session::onStreamDestroyed()
{
    IFileStream *stream = qobject_cast<IFileStream *>(sender());
    if (stream)
    {
        //qDebug() << "onStreamDestroyed: " << stream->fileName();
        //QString filename = stream->fileName();
        //QFile *file = new QFile(Options::node(OPV_FILESTREAMS_DEFAULTDIR).value().toString() + QDir::separator() + filename);
        //if (file->exists())
        //    file->rename(FCurrentDirectory + QDir::separator() + stream->fileName());
        //FCurrentDirectory = "";
    }
}

void Session::onStarted()
{
    writePWD();
}


