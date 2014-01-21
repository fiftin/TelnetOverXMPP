#include "historywidget.h"

HistoryWidget::HistoryWidget(QWidget *parent) :
    QTextEdit(parent)
{
    setReadOnly(true);
}

void HistoryWidget::keyPressEvent(QKeyEvent *evt)
{
    emit keyPressed(evt);
}

