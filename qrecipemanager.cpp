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
#include "qrecipemanager.h"
#include "qmysettings.h"
#include <QProcess>

QRecipeManager::QRecipeManager(QObject *parent) :
    QObject(parent)
{
    LoadRecipes();
}

int QRecipeManager::GetRecipeCount()
{
    return m_recipes.count();
}

QRecipe* QRecipeManager::GetRecipe(int idx)
{
    if(idx < m_recipes.count())
        return m_recipes[idx];

    return NULL;
}

QList<QRecipe*>* QRecipeManager::GetRecipeList()
{
    return &m_recipes;
}

void QRecipeManager::AddRecipe(QRecipe *pRec)
{
    int i,cnt;
    QRecipe *pNewRec = new QRecipe(pRec->GetName(), this);
    QRecipeEntry *pRecEntry;

    cnt = pRec->GetEntryCount();
    for(i=0;i<cnt;i++)
    {
        pRecEntry = pRec->GetEntry(i);
        pNewRec->AddEntry(pRecEntry->GetName(), pRecEntry->GetTemp(), pRecEntry->GetTime(), NULL);
    }

    m_recipes.append(pNewRec);
}

void QRecipeManager::DeleteRecipe(int idx)
{
    if(idx < m_recipes.count())
    {
        m_recipes.removeAt(idx);
        SaveRecipes();
    }
}

bool QRecipeManager::RecipeExists(QRecipe *pRec)
{
    return (m_recipes.lastIndexOf(pRec) != -1);
}

void QRecipeManager::ModifyRecipe(QRecipe *pRec)
{
    if(!RecipeExists(pRec))
        AddRecipe(pRec);
    SaveRecipes();
}

void QRecipeManager::stripRecipeEntries(QRecipe *pRec, int newcount)
{
    int i,cnt;
    QRecipeEntry *pRecEntry;

    cnt = pRec->GetEntryCount();
    for(i = cnt; i>newcount; i--)
    {
        pRec->RemoveEntry(i-1);
    }
}

void QRecipeManager::AddRecipeEntry(QRecipe *pRec, QString name, double temp, int time, QPIDParameters *pParams)
{
    pRec->AddEntry(name, temp, time, pParams);
}

void QRecipeManager::LoadRecipes()
{
    XMLElement *akt;
    XMLElement *step;
    XMLElement *elem;
    QRecipe *pRec = NULL;
    QPIDParameters *pParams = NULL;
    QString name;
    double temp;
    int time;

    if(m_recipedoc.LoadFile(RECIPE_FILE) != XML_ERROR_FILE_NOT_FOUND)
    {
        akt = m_recipedoc.FirstChildElement("Recipes");
        if(akt != NULL)
            akt = akt->FirstChildElement("Recipe");
        while(akt)
        {
            pRec = new QRecipe(akt->FirstChildElement("Name")->GetText(), this);

            step = akt->FirstChildElement("Steps");
            if(step)
                step = step->FirstChildElement("Step");
            while(step)
            {
                elem = step->FirstChildElement("Name");
                if(elem != NULL)
                {
                    name = elem->GetText();
                    elem = step->FirstChildElement("Temp");
                    if(elem != NULL)
                    {
                        temp = atof(elem->GetText());
                        elem = step->FirstChildElement("Time");
                        if(elem != NULL)
                        {
                            time = atoi(elem->GetText());

/*                            elem = step->FirstChildElement("PIDParams");
                            if(elem != NULL)
                            {
                                pParams = GetPidParams(elem);
                            }*/
                            pRec->AddEntry(name, temp, time, pParams);
                            pParams = NULL;
                        }
                    }
                }
                step = step->NextSiblingElement("Step");
            }
            m_recipes.append(pRec);
            akt = akt->NextSiblingElement("Recipe");
        }
    }
}

void QRecipeManager::SaveRecipes()
{
    int i;
    XMLDocument* doc = new XMLDocument();

    XMLNode* element = doc->InsertEndChild( doc->NewElement( "Recipes" ) );
    for(i=0;i<m_recipes.count();i++)
        m_recipes[i]->serialize(element, doc);
    doc->SaveFile(RECIPE_FILE);
#ifndef Q_OS_WIN
    QProcess::execute("/bin/sync");
#endif
    delete doc;
}
