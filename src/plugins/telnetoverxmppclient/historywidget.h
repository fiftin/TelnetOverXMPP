#ifndef HISTORYWIDGET_H
#define HISTORYWIDGET_H

#include <QTextEdit>

class HistoryWidget : public QTextEdit
{
    Q_OBJECT
public:
    explicit HistoryWidget(QWidget *parent = 0);
protected:
    virtual void keyPressEvent(QKeyEvent *evt);
signals:
    void keyPressed(QKeyEvent *evt);

public slots:
    
};

#endif // HISTORYWIDGET_H
