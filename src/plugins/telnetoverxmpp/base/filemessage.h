#ifndef FILEMESSAGE_H
#define FILEMESSAGE_H
#include <QString>
#include <QByteArray>
#include <QTextCodec>
class Message2;
class ConnectionInfo;

enum FileMessageMethod {
    FMM_INTERNAL = 0,
    FMM_FILETRANSFER = 10000
};

class FileMessage : public Message2
{
    static QTextCodec *FCodec;
public:
    static FileMessage createFile(const ConnectionInfo& AInfo, const QString& AFilename, const QByteArray& AInternalContent);
    static FileMessage createFile(const ConnectionInfo& AInfo, const QString& AFilename, const QByteArray& AInternalContent,  FileMessageMethod AMethod);
    static FileMessage createFile(const ConnectionInfo& AInfo, const QString& AFilename, const QString& AInternalContent,  FileMessageMethod AMethod);

    QString fileName() const;
    FileMessageMethod getMethod() const;
    QByteArray internalContent() const;
    QString internalContentStr() const;
    QByteArray loadContent() const;
    void saveContent(QByteArray AContent);
    FileMessage(const Message2& AMessage);
    static bool isFile(const Message2& AMessage);
};

#endif // FILEMESSAGE_H
