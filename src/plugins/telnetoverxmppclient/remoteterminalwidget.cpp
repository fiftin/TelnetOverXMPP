#include "remoteterminalwidget.h"
#include "ui_remoteterminalwidget.h"
#include <QKeyEvent>
#include <QMessageBox>
#include <QDebug>
#include <QScrollBar>
#include <QMenu>
#include <QFileDialog>
#include "../telnetoverxmpp/base/filemessage.h"
RemoteTerminalWidget::RemoteTerminalWidget(Connection* AConnection, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RemoteTerminalWidget),
    FWaitingResponse(false),
    FConnection(AConnection)
{
    ui->setupUi(this);
    connect(FConnection, SIGNAL(connected()), SLOT(fConnection_connected()));
    connect(FConnection, SIGNAL(closed(ConnectionClosingReason)), SLOT(fConnection_closed(ConnectionClosingReason)));
    connect(FConnection, SIGNAL(received(Message2)), SLOT(fConnection_received(Message2)));
    connect(FConnection, SIGNAL(connectionError(ConnectionError)), SLOT(fConnection_error(ConnectionError)));
    FConnection->open();
}

RemoteTerminalWidget::~RemoteTerminalWidget()
{
    delete FConnection;
    delete ui;
}

void RemoteTerminalWidget::sendCommand(const QString& ACommand)
{
    connection()->send(ACommand);
}

void RemoteTerminalWidget::setFocusToCommandLine()
{
    ui->txtCommand->setFocus();
}

void RemoteTerminalWidget::on_btnSend_clicked()
{
    sendCommand(ui->txtCommand->text());
    ui->txtCommand->clear();
    ui->txtCommand->setFocus();
    desableSend();
    FWaitingResponse = true;
}

void RemoteTerminalWidget::on_txtCommand_returnPressed()
{
    on_btnSend_clicked();
}

void RemoteTerminalWidget::fConnection_closed(ConnectionClosingReason AReason)
{
    //ui->txtHistory->append("");
    ui->txtHistory->append(tr("<font color='blue'>Connection closed</font>"));
    desableSend();
}

void RemoteTerminalWidget::fConnection_received(const Message2& AMessage)
{
    if (FileMessage::isFile(AMessage)) {
        FileMessage message(AMessage);
        QString dir = QFileDialog::getExistingDirectory(this, tr("Select directory for save a file."));
        if (!dir.isNull()) {
            QFile *file = new QFile(dir+QDir::separator()+message.fileName());
            if (file->open(QFile::WriteOnly)) {
                file->write(message.internalContent());
                file->close();
                ui->txtHistory->append("File '" + message.fileName() + "' saved on <b>local</b> PC.");
            }
            delete file;
        }
    }
    else {
        QTextCursor cur = ui->txtHistory->textCursor();
        cur.movePosition(QTextCursor::End);
        ui->txtHistory->setTextCursor(cur);
        ui->txtHistory->insertPlainText(AMessage.data());
        ui->txtHistory->verticalScrollBar()->setValue(ui->txtHistory->verticalScrollBar()->maximum());
    }
    if (FWaitingResponse) {
        enableSend();
    }
}

void RemoteTerminalWidget::fConnection_connected()
{
    enableSend();
}

void RemoteTerminalWidget::fConnection_error(const ConnectionError &AError)
{
    ui->txtHistory->append("");
    ui->txtHistory->append(tr("Connection error: ")+AError.message());
}

void RemoteTerminalWidget::focusInEvent(QFocusEvent *evt)
{
    setFocusToCommandLine();
    QWidget::focusInEvent(evt);
}

void RemoteTerminalWidget::on_txtHistory_keyPressed(QKeyEvent *evt)
{
    if (evt->modifiers() == Qt::NoModifier
            || (evt->modifiers()&Qt::ShiftModifier)==Qt::ShiftModifier) {
        ui->txtCommand->event(evt);
        ui->txtCommand->setFocus();
        //evt->ignore();
    }
}

void RemoteTerminalWidget::on_btnMore_clicked()
{
    QMenu* contextMenu = new QMenu(this);
    QList<QAction*> actions;
    QAction* ctrlC = new QAction("Ctrl+C", contextMenu);
    QAction* sendFile = new QAction(tr("Short File"), contextMenu);
    QAction* sendBigFile = new QAction(tr("Big File"), contextMenu);
    connect(ctrlC, SIGNAL(triggered()), SLOT(onSendCtrlC()));
    connect(sendFile, SIGNAL(triggered()), SLOT(onSendShortFile()));
    connect(sendBigFile, SIGNAL(triggered()), SLOT(onSendBigFile()));
    actions.append(sendFile);
    actions.append(sendBigFile);
    actions.append(ctrlC);
    contextMenu->addActions(actions);
    contextMenu->exec(mapToGlobal(QPoint(ui->btnMore->pos().x()+ui->btnMore->size().width(),
                                        ui->btnMore->pos().y())));
}

void RemoteTerminalWidget::onSendCtrlC()
{
    this->FConnection->sendControlC();
}

void RemoteTerminalWidget::onSendBigFile()
{
    this->FConnection->sendBigFile();
    desableSend();
    FWaitingResponse = true;
}
void RemoteTerminalWidget::onSendShortFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select File"), "", tr("Files (*.*)"));
    this->FConnection->sendFile(fileName);
    desableSend();
    FWaitingResponse = true;
}

void RemoteTerminalWidget::enableSend()
{
    //ui->btnSend->setEnabled(true);
    //ui->txtCommand->setEnabled(true);
    //ui->btnMore->setEnabled(true);
    //ui->txtCommand->setFocus();
}

void RemoteTerminalWidget::desableSend()
{
    //ui->btnSend->setEnabled(false);
    //ui->txtCommand->setEnabled(false);
    //ui->btnMore->setEnabled(false);
}
