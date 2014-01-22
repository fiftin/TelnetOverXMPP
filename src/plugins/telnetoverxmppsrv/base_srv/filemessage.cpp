#include "message2.h"
#include "filemessage.h"
#include <QDebug>

QTextCodec *FileMessage::FCodec = NULL;

FileMessage FileMessage::createFile(const ConnectionInfo& AInfo, const QString& AFilename, const QByteArray& AInternalContent)
{
    return createFile(AInfo, AFilename, AInternalContent, FMM_INTERNAL);
}

FileMessage FileMessage::createFile(const ConnectionInfo &AInfo, const QString &AFilename, const QByteArray &AInternalContent, FileMessageMethod AMethod)
{
    QString data = QString::fromAscii(AInternalContent.toBase64());
    FileMessage result(Message2::createDataMessage(AInfo, data));
    result.setAttr("type", "file");
    result.setAttr("method", QString::number(AMethod));
    result.setAttr("filename", AFilename);
    return result;
}

FileMessage FileMessage::createFile(const ConnectionInfo &AInfo, const QString &AFilename, const QString &AInternalContent, FileMessageMethod AMethod)
{
    if (FCodec == NULL)
        FCodec = QTextCodec::codecForName("UTF-8");
    QByteArray content = FCodec->fromUnicode(AInternalContent);
    return createFile(AInfo, AFilename, content, AMethod);
}

QString FileMessage::fileName() const
{
    return attr("filename");
}

FileMessageMethod FileMessage::getMethod() const
{
    return (FileMessageMethod)attr("method").toInt();
}

FileMessage::FileMessage(const Message2& AMessage)
    : Message2(AMessage.stanza())
{
}

bool FileMessage::isFile(const Message2& AMessage)
{
    return AMessage.isDataMessage() &&  AMessage.attr("type") == "file";
}

QByteArray FileMessage::internalContent() const
{
    return QByteArray::fromBase64(data().toAscii());
}

QString FileMessage::internalContentStr() const
{
    if (FCodec == NULL)
        FCodec = QTextCodec::codecForName("UTF-8");
    return FCodec->toUnicode(internalContent());
}

QByteArray FileMessage::loadContent() const
{
    if (getMethod() == FMM_INTERNAL)
        return "";
    //MegaClient* client = new MegaClient(new MyMegaApp,new CurlHttpIO,new BdbAccess,"TELNETOVERXMPP");
    //client
    return QByteArray("");
}

void FileMessage::saveContent(QByteArray AContent)
{
    //MegaClient* client = new MegaClient(new MyMegaApp,new CurlHttpIO,new BdbAccess,"TELNETOVERXMPP");
    switch (getMethod()) {
    case FMM_FILETRANSFER:
        break;
    default:
        break;
    }
}
