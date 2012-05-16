//
//  SceneModel.cpp
//  qsculpt
//
//  Created by Juan Cabral on 3/3/12.
//  Copyright (c) 2012 plastilinaware. All rights reserved.
//

#include <iostream>
#include "SceneModel.h"
#include "SceneNode.h"

struct SceneModel::Impl {
    Scene::SharedPtr scene;
    
    Impl(Scene::SharedPtr & p) : scene(p) 
    {}
};

SceneModel::SceneModel(Scene::SharedPtr scene, QObject * parent)
: QAbstractItemModel(parent),
d_(new Impl(scene))
{
    
}

SceneModel::~SceneModel()
{
    
}

Scene::SharedPtr SceneModel::scene()
{
    return d_->scene;
}

Scene::const_shared_ptr SceneModel::scene() const
{
    return d_->scene;
}


void SceneModel::setScene(const Scene::SharedPtr & scene)
{
    d_->scene = scene;
}

int SceneModel::columnCount(const QModelIndex & parent) const
{
    return 1;
}

QVariant SceneModel::data (const QModelIndex & index, 
              int role ) const
{
    return QVariant::Invalid;
}

QModelIndex SceneModel::index (int row, 
                           int column, 
                           const QModelIndex & parent ) const
{
    assert(d_ && d_->scene);
    
    SceneNode::SharedPtr p = NULL;
    if (parent.isValid()) {
        p = d_->scene->findByIID(parent.internalId());
    }
    if (p) {
        p = p->item(row).lock();
    } else if (d_->scene) {
        p = d_->scene->item(row).lock();
    }
    if (!p) {
        return createIndex(-1, -1, 0);
    }
    return createIndex(row, 0, p->iid());
}

QModelIndex SceneModel::parent ( const QModelIndex & index ) const
{
    assert(d_ && d_->scene);
    
    if (!index.isValid()) {
        throw std::runtime_error("Invalid index");
    }
    
    SceneNode::SharedPtr p = d_->scene->findByIID(index.internalId());
    int row = -1;
    
    if (p) {
        size_t n = 0;
        row = p->itemIndex(p, &n) ? n : -1;
    } else {
        if (d_->scene) {
            p = d_->scene->item(row).lock();
            size_t n = 0;
            row = p->itemIndex(p, &n) ? n : -1;
        }
    }
    if (!p) {
        return createIndex(-1, -1, 0);
    }
    return createIndex(row, 0, p->iid());
}

int SceneModel::rowCount ( const QModelIndex & parent ) const
{
    assert(d_ && d_->scene);

    if (parent.isValid()) {
        SceneNode::SharedPtr p = d_->scene->findByIID(parent.internalId());
        if (p) {
            return p->count();
        }
    }
    if (d_->scene) {
        return d_->scene->count();
    }
    return 0;
}