#ifndef QSSRRELAY_H
#define QSSRRELAY_H

#include <QThread>

class QSSRrelay : public QThread
{
    Q_OBJECT
public:
    explicit QSSRrelay(QObject *parent = 0) :QThread(parent) {};
    virtual void Start() = 0;
    virtual void SetPower(int power) = 0;

signals:
    
public slots:
    
};

#endif // QSSRRELAY_H
