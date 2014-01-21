#include "remoteterminaldialog.h"
#include "ui_remoteterminaldialog.h"

#include <QMessageBox>
#include "remoteterminalwidget.h"

RemoteTerminalDialog::RemoteTerminalDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RemoteTerminalDialog)
{
    ui->setupUi(this);
}

RemoteTerminalDialog::~RemoteTerminalDialog()
{
    delete ui;
}

void RemoteTerminalDialog::addTerminal(Connection *AConnection)
{
    RemoteTerminalWidget *terminal = new RemoteTerminalWidget(AConnection);
    ui->terminalTabControl->addTab(terminal, AConnection->remoteJid().full());
    ui->terminalTabControl->setCurrentWidget(terminal);
}

void RemoteTerminalDialog::showDialog()
{
    if (!isVisible())
        showNormal();
    else {
        raise();
        activateWindow();
    }
}

void RemoteTerminalDialog::on_terminalTabControl_tabCloseRequested(int index)
{
    RemoteTerminalWidget *widget = qobject_cast<RemoteTerminalWidget*>(ui->terminalTabControl->widget(index));
    ui->terminalTabControl->removeTab(index);
    delete widget;
    if (ui->terminalTabControl->count() == 0)
        close();
}

void RemoteTerminalDialog::on_terminalTabControl_currentChanged(QWidget *arg1)
{
    RemoteTerminalWidget* term = qobject_cast<RemoteTerminalWidget*>(arg1);
    if (term != NULL)
        term->setFocusToCommandLine();
}
