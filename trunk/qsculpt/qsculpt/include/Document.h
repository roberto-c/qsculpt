/***************************************************************************
 *   Copyright (C) 2006 by Juan Roberto Cabral Flores   *
 *   roberto.cabral@gmail.com   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <QList>
#include "IDocument.h"
#include "IIterator.h"

class ISurface;

/**
 * Holds all the document data.
 *
 * @author Juan Roberto Cabral Flores <roberto.cabral@gmail.com>
*/
class Document : public IDocument
{
public:
     
    Document();

    virtual ~Document();

// IDocument interface
    virtual void loadFile(const QString& fileName);
    virtual void saveFile(const QString& fileName);
    virtual void selectObject(int iid);
    virtual QList<SceneNode*> getSelectedObjects() const;
    virtual SceneNode* rootNode();
    virtual const SceneNode* rootNode() const;
    virtual Scene* scene();
    virtual Scene* scene() const;
    virtual Iterator<SceneNode> sceneIterator();
    virtual Iterator<SceneNode> constSceneIterator() const;
    Iterator<ISurface> surfaceIterator();
// End IDocument interface
    
private:
    struct Impl;
    QScopedPointer<Impl>        _d;
};

#endif

