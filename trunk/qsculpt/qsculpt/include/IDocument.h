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
#ifndef IDOCUMENT_H
#define IDOCUMENT_H

#include <QObject>

class IObject3D;

/**
 * Interface that should implement every kind of document.
 *
 * @author Juan Roberto Cabral Flores <roberto.cabral@gmail.com>
 */
class IDocument : public QObject
{
    Q_OBJECT

public:
    enum ObjectType {
        Mesh,
        Box,
        Sphere
    };
    
    enum ChangeType {
        AddObject,
        RemoveObject,
        ObjectChanged
    };
    
    /**
     * IDocument default constructor.
     */
    IDocument(){}

    /**
     * Default virtual destructor.
     */
    virtual ~IDocument(){}
    
    /**
     * Load file
     */
    virtual void loadFile(QString fileName) = 0;
    
    /**
     * Save file
     */
    virtual void saveFile(QString fileName) = 0;
    
    /**
     * Adds an object to the document.
     *
     * @param type type of object to add.
     */
    virtual void addObject(ObjectType type) = 0;
    
    /**
     * Remove an object from the document. This does not free the memory allocated
     * by the object.
     *
     * @param index index of the object to remove.
     */
    virtual void removeObject(int index) = 0;
    
    /**
     * Gets the Nth object in the document.
     *
     * @param index index of object to retrieve
     *
     * @return an object with IObject3D interface
     */
    virtual IObject3D* getObject(int index) const = 0;
    
    /**
     * Get the objects count on this document.
     *
     * @return count of objects on the document
     */
    virtual int getObjectsCount() const = 0;

    /**
     * 
     */
    virtual void selectObject(int index) = 0;
    
    /**
     * 
     */
    virtual QList<IObject3D*> getSelectedObjects() const = 0;
    
signals:
    void changed(IDocument::ChangeType type, IObject3D* object);
};

#endif

