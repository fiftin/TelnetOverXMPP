#include "sessionsdialog.h"
#include "ui_sessionsdialog.h"

#include "telnetoverxmppsrv.h"
SessionsDialog::SessionsDialog(TelnetOverXmppSrv *AServer, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SessionsDialog),
    FServer(AServer),
    FModel(NULL)
{
    ui->setupUi(this);
    FModel = new QStandardItemModel(FServer->getConnections().length(), 2, this);
    connect(ui->lstSessions, SIGNAL(activated(QModelIndex)), SLOT(onItemActived(QModelIndex)));
    ui->lstSessions->setModel(FModel);
}

SessionsDialog::~SessionsDialog()
{
    delete ui;
}

void SessionsDialog::showDialog()
{
    if (!isVisible())
        showNormal();
    else {
        raise();
        activateWindow();
    }
    FModel->clear();
    foreach (ConnectionBase *c, FServer->getConnections()) {
        Session *s = static_cast<Session*>(c);
        QStandardItem *item = new QStandardItem(s->remoteJid().full());
        item->setData(QVariant(s->info().sid));
        FModel->appendRow(item);
    }
}

void SessionsDialog::activated(const QModelIndex &index)
{
    QVariant value = index.data(Qt::UserRole+1);
    QString sid = value.toString();
    ConnectionBase *conn = this->FServer->getConnection(sid);
    if (conn != NULL) {
        ui->txtStreamJid->setText(conn->jid().full());
        ui->txtContactJid->setText(conn->remoteJid().full());
        ui->txtSessionId->setText(conn->sid());
        ui->txtConnectionId->setText(conn->remoteSid());
    }
}
