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
#include "qrecipe.h"

QRecipe::QRecipe(QString name, QObject *parent) :
    QObject(parent)
{
    m_name = name;
}

void QRecipe::AddEntry(QString name, double temp, int time, QPIDParameters *pParams)
{
    QRecipeEntry *entry;

    entry = new QRecipeEntry(name, temp, time, pParams, this);
    if(pParams != NULL)
        pParams->setParent(entry);
    m_entries.append(entry);
}

void QRecipe::RemoveEntry(int idx)
{
    if(idx < m_entries.count())
        m_entries.removeAt(idx);
}

int QRecipe::GetEntryCount()
{
    return m_entries.count();
}

QRecipeEntry* QRecipe::GetEntry(int idx)
{
    if(idx < m_entries.count())
        return m_entries[idx];

    return NULL;
}

QString QRecipe::GetName()
{
    return m_name;
}

void QRecipe::SetName(QString name)
{
    m_name = name;
}

void QRecipe::serialize(QXmlStreamWriter &writer)
{
    int i;

    writer.writeStartElement("Recipe");
    writer.writeTextElement("Name", m_name);

    for(i=0;i<m_entries.count();i++)
        m_entries[i]->serialize(writer);
    writer.writeEndElement();
}

void QRecipe::serialize(XMLNode *parent, XMLDocument* doc)
{
    int i;
    XMLElement *elem;
    parent = parent->InsertEndChild(doc->NewElement("Recipe"));
    if(parent)
    {
        elem = doc->NewElement("Name");
        elem->InsertFirstChild(doc->NewText(m_name.toStdString().c_str()));
        parent->InsertEndChild(elem);

        parent = parent->InsertEndChild(doc->NewElement("Steps"));
        for(i=0;i<m_entries.count();i++)
            m_entries[i]->serialize(parent, doc);
    }
}
