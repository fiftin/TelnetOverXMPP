#ifndef SESSIONSDIALOG_H
#define SESSIONSDIALOG_H

#include <QDialog>
#include "telnetoverxmppsrv.h"

namespace Ui {
class SessionsDialog;
}

class SessionsDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit SessionsDialog(TelnetOverXmppSrv *AServer, QWidget *parent = 0);
    ~SessionsDialog();
    
private:
    Ui::SessionsDialog *ui;
    TelnetOverXmppSrv *FServer;
};

#endif // SESSIONSDIALOG_H
