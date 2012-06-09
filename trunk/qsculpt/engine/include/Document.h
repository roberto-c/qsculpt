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

#include <QtCore/QList>
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
    typedef std::shared_ptr<Document>   shared_ptr;
    typedef std::shared_ptr<const Document>   const_shared_ptr;
    typedef std::weak_ptr<Document>     weak_ptr;
    typedef std::weak_ptr<const Document>  const_weak_ptr;
    typedef std::unique_ptr<Document>   Ptr;
    
    Document();

    virtual ~Document();

// IDocument interface
    virtual void loadFile(const QString& fileName);
    virtual void saveFile(const QString& fileName);
    virtual void selectObject(int iid);
    virtual QList<SceneNode::weak_ptr> getSelectedObjects() const;
    virtual Scene::weak_ptr scene();
    virtual Scene::weak_ptr scene() const;
    virtual Iterator<SceneNode> sceneIterator();
    virtual Iterator<SceneNode> constSceneIterator() const;
    Iterator<ISurface> surfaceIterator();
    
    virtual SceneNode::shared_ptr findItem(uint iid);
    
    /**
     * Returns the number of columns for this model.
     *
     * From QAbstractItemModel
     */
    virtual int	columnCount ( const QModelIndex & parent = QModelIndex() ) const;
    
    virtual QVariant	data ( const QModelIndex & index, 
                              int role = Qt::DisplayRole ) const;
    
    virtual QModelIndex	index (int row, 
                               int column, 
                               const QModelIndex & parent = QModelIndex() ) const;
    
    virtual QModelIndex	parent ( const QModelIndex & index ) const;
    
    virtual int	rowCount ( const QModelIndex & parent = QModelIndex() ) const;
    
    virtual bool insertRow ( int row, const QModelIndex & parent = QModelIndex() );
    
    virtual void addItem(SceneNode::shared_ptr node, 
                         const QModelIndex & parent = QModelIndex());
    
    virtual bool setData (const QModelIndex & index,
                            const QVariant & value, 
                          int role = Qt::EditRole );
    
    virtual Qt::ItemFlags flags ( const QModelIndex & index ) const;

// End IDocument interface
    
private:
    struct Impl;
    QScopedPointer<Impl>        _d;
};

#endif

