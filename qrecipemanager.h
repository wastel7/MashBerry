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
#ifndef QRECIPEMANAGER_H
#define QRECIPEMANAGER_H

#include "qrecipe.h"

class QRecipeManager : public QObject
{
    Q_OBJECT
public:
    explicit QRecipeManager(QObject *parent = 0);
    void SaveRecipes();
    QRecipe *GetRecipe(int idx);
    int GetRecipeCount();
    QList<QRecipe*>* GetRecipeList();
    void AddRecipe(QRecipe *pRec);
    bool RecipeExists(QRecipe *pRec);

signals:
    
public slots:
    void ModifyRecipe(QRecipe *pRec);
    void stripRecipeEntries(QRecipe *pRec, int newcount);
    void AddRecipeEntry(QRecipe *pRec, QString name, double temp, int time, QPIDParameters *pParams);
    void DeleteRecipe(int idx);

protected:
    void LoadRecipes();
    QPIDParameters *GetPidParams(XMLElement *parent);

private:
    XMLDocument m_recipedoc;
    QList<QRecipe*> m_recipes;
};

#endif // QRECIPEMANAGER_H
