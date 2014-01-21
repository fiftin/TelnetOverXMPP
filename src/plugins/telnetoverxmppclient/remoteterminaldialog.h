#ifndef REMOTETERMINALDIALOG_H
#define REMOTETERMINALDIALOG_H

#include <QDialog>
#include "remoteterminalwidget.h"

namespace Ui {
class RemoteTerminalDialog;
}
class Connection;
class IStanzaProcessor;
class RemoteTerminalWidget;

class RemoteTerminalDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit RemoteTerminalDialog(QWidget *parent = 0);
    ~RemoteTerminalDialog();

    void addTerminal(Connection* AConnection);
    void showDialog();

private slots:


    void on_terminalTabControl_tabCloseRequested(int index);


    void on_terminalTabControl_currentChanged(QWidget *arg1);

private:
    Ui::RemoteTerminalDialog *ui;

};

#endif // REMOTETERMINALDIALOG_H
