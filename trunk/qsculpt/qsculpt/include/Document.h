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

class IObject3D;

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
    virtual void loadFile(QString fileName);
    virtual void saveFile(QString fileName);
    virtual void addObject(ObjectType type);
    virtual void removeObject(int index);
    virtual IObject3D* getObject(int index) const;
    virtual int getObjectsCount() const;
    virtual void selectObject(int index);
    virtual QList<IObject3D*> getSelectedObjects() const;
// End IDocument interface
    
private:
    QList<IObject3D*> m_objectList;
    QList<IObject3D*> m_selectedObjectList;
};

#endif

