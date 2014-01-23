#ifndef TERMINALHISTORYWIDGET_H
#define TERMINALHISTORYWIDGET_H

#include <QTextEdit>
#include <QDebug>

class TerminalHistoryWidget : public QTextEdit
{
    Q_OBJECT
public:
    explicit TerminalHistoryWidget(QWidget *parent = 0);
    virtual ~TerminalHistoryWidget() { }
protected:
    virtual void keyPressEvent(QKeyEvent *evt);
signals:
    void keyPressed(QKeyEvent *evt);

public slots:
    
};

#endif // TERMINALHISTORYWIDGET_H
