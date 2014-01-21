#include "commandlinewidget.h"
#include <QKeyEvent>
#include <QDebug>
CommandLineWidget::CommandLineWidget(QWidget *parent) :
    QLineEdit(parent),
    FCurrentHistoryIndex(-1)
{
}

void CommandLineWidget::keyPressEvent(QKeyEvent *evt)
{
    if (evt->key() == Qt::Key_Enter || evt->key() == Qt::Key_Return) {
        if (FHistory.size() == 0 || FHistory[0] != text())
            FHistory.push_front(text());

        if (FHistory.size() > 50)
            FHistory.pop_back();
        if (FCurrentHistoryIndex != -1 && FHistory[FCurrentHistoryIndex] != text())
            FCurrentHistoryIndex = -1;
    }
    else if (evt->key() == Qt::Key_Up) {
        if (FHistory.size() > 0)
        {
            if (FCurrentHistoryIndex < FHistory.size()-1) {
                FCurrentHistoryIndex++;
                setText(FHistory[FCurrentHistoryIndex]);
            }
        }
    }
    else if (evt->key() == Qt::Key_Down) {
        if (FHistory.size() > 0)
        {
            if (FCurrentHistoryIndex > 0) {
                FCurrentHistoryIndex--;
                setText(FHistory[FCurrentHistoryIndex]);
            }
        }
    }

    QLineEdit::keyPressEvent(evt);
}
