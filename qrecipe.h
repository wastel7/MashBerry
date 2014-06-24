/*
 *
 * MashBerry Homebrewing-controller
 *
 * Copyright (C) 2013 Sebastian Duell
 * <mail@sebastian-duell.de>
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 3. This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */
#ifndef QRECIPE_H
#define QRECIPE_H

#include <QObject>
#include <QList>
#include "qrecipeentry.h"

class QRecipe : public QObject
{
    Q_OBJECT
public:
    explicit QRecipe(QString name, QObject *parent = 0);
    void serialize(QXmlStreamWriter &writer);
    void serialize(XMLNode *parent, XMLDocument* doc);

    void AddEntry(QString name, double temp, int time, QPIDParameters *pParams);
    void RemoveEntry(int idx);
    int GetEntryCount();
    QRecipeEntry* GetEntry(int idx);
    QString GetName();
    void SetName(QString name);

signals:
    
public slots:
    
private:
    QString m_name;
    QList<QRecipeEntry*> m_entries;
};

#endif // QRECIPE_H
