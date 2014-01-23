#include "sessionsdialog.h"
#include "ui_sessionsdialog.h"

SessionsDialog::SessionsDialog(TelnetOverXmppSrv *AServer, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SessionsDialog),
    FServer(AServer)
{
    ui->setupUi(this);
    foreach (ConnectionBase *c, FServer->getConnections()) {
        Session *s = static_cast<Session>(c);
    }
}

SessionsDialog::~SessionsDialog()
{
    delete ui;
}
