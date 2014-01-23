#ifndef SESSIONSDIALOG_H
#define SESSIONSDIALOG_H

#include <QDialog>
#include <QStandardItemModel>
#include "session.h"
class TelnetOverXmppSrv;
namespace Ui {
class SessionsDialog;
}

class SessionsDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit SessionsDialog(TelnetOverXmppSrv *AServer, QWidget *parent = 0);
    ~SessionsDialog();
    void showDialog();
private slots:
    void onItemActived(const QModelIndex &index);

private:
    Ui::SessionsDialog *ui;
    TelnetOverXmppSrv *FServer;
    QStandardItemModel *FModel;
};

#endif // SESSIONSDIALOG_H
