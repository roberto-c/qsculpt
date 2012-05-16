//
//  SceneModel.h
//  qsculpt
//
//  Created by Juan Cabral on 3/3/12.
//  Copyright (c) 2012 plastilinaware. All rights reserved.
//
#pragma once

#ifndef qsculpt_SceneModel_h
#define qsculpt_SceneModel_h

#include "Scene.h"
#include <QtCore/QAbstractItemModel>

class SceneModel : public QAbstractItemModel
{
public:
    SceneModel(Scene::SharedPtr scene = NULL, QObject * parent = NULL);
    
    virtual ~SceneModel();
  
    /**
     * Returns the number of columns for this model.
     */
    virtual int	columnCount ( const QModelIndex & parent = QModelIndex() ) const;
    
    virtual QVariant	data ( const QModelIndex & index, 
                              int role = Qt::DisplayRole ) const;
    
    virtual QModelIndex	index (int row, 
                               int column, 
                               const QModelIndex & parent = QModelIndex() ) const;
    
    virtual QModelIndex	parent ( const QModelIndex & index ) const;
    
    virtual int	rowCount ( const QModelIndex & parent = QModelIndex() ) const;
    
    /**
     * Returns the scene object this model uses to retrieve data from.
     */
    Scene::SharedPtr scene();
    
    Scene::const_shared_ptr scene() const;
    
    /**
     * Sets the scene that the model makes reference to.
     */
    void setScene(const Scene::SharedPtr & scene);
    
private:
    struct Impl;
    QScopedPointer<Impl> d_;
};

#endif
