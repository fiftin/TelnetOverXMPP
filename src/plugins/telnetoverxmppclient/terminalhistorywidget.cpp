#include "terminalhistorywidget.h"

TerminalHistoryWidget::TerminalHistoryWidget(QWidget *parent) :
    QTextEdit(parent)
{
    setReadOnly(true);
}

void TerminalHistoryWidget::keyPressEvent(QKeyEvent *evt)
{
    QTextEdit::keyPressEvent(evt);
    emit keyPressed(evt);
}

