#ifndef REMOTETERMINALWIDGET_H
#define REMOTETERMINALWIDGET_H

#include <QWidget>

#include "connection.h"

namespace Ui {
class RemoteTerminalWidget;
}

class RemoteTerminalWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit RemoteTerminalWidget(Connection *AConnection, QWidget *parent = 0);
    ~RemoteTerminalWidget();

    Connection* connection() { return FConnection; }

    void sendCommand(const QString& str);
    void setFocusToCommandLine();
private slots:
    void on_btnSend_clicked();
    void on_txtCommand_returnPressed();
    void on_txtHistory_keyPressed(QKeyEvent *evt);

    void fConnection_closed(ConnectionClosingReason);
    void fConnection_received(const Message2&);
    void fConnection_connected();
    void fConnection_error(const ConnectionError &AError);
    void on_btnMore_clicked();
    void onSendCtrlC();
    void onSendBigFile();
    void onSendShortFile();
protected:
    void enableSend();
    void desableSend();
    //virtual void keyPressEvent(QKeyEvent *evt);
    //virtual void keyReleaseEvent(QKeyEvent *evt);
    virtual void focusInEvent(QFocusEvent *evt);
private:
    Ui::RemoteTerminalWidget *ui;
    bool FWaitingResponse;
    Connection *FConnection;
};

#endif // REMOTETERMINALWIDGET_H
