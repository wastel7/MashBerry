#ifndef QSSRRELAYFACTORY_H
#define QSSRRELAYFACTORY_H

#include <QObject>
#include "QSSRrelay.h"

class QSSRrelayFactory : public QObject
{
    Q_OBJECT
public:
    explicit QSSRrelayFactory(QObject *parent = 0);
    QSSRrelay* GetSSRrelay(int ssr);

signals:
    
public slots:
    
};

#endif // QSSRRELAYFACTORY_H
