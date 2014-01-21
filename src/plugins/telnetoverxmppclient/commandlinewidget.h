#ifndef COMMANDLINEWIDGET_H
#define COMMANDLINEWIDGET_H

#include <QLineEdit>

class CommandLineWidget : public QLineEdit
{
    Q_OBJECT
public:
    explicit CommandLineWidget(QWidget *parent = 0);
protected:
    virtual void keyPressEvent(QKeyEvent *evt);
signals:
    
public slots:

private:
    QList<QString> FHistory;
    int FCurrentHistoryIndex;
};

#endif // COMMANDLINEWIDGET_H
