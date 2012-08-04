//
//  DocumentModel.cpp
//  qsculpt
//
//  Created by Juan Cabral on 7/15/12.
//  Copyright (c) 2012 plastilinaware. All rights reserved.
//

#include <iostream>
#include "DocumentModel.h"

#include <PlastilinaCore/Document.h>
#include <PlastilinaCore/Scene.h>
#include <PlastilinaCore/SceneNode.h>

int DocumentModel::columnCount(const QModelIndex & parent) const
{
    TRACEFUNCTION();
    //    qDebug() << "Args1: " << parent;
    //    qDebug() << 1;
    return 1;
}

Qt::ItemFlags DocumentModel::flags ( const QModelIndex & index ) const
{
    TRACEFUNCTION();
    //qDebug() << "Args1: " << index;
    //return QAbstractItemModel::flags(index);
    auto ret = QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
    //qDebug() << ret;
    return ret;
}

QVariant DocumentModel::data (const QModelIndex & index, 
                         int role ) const
{
    TRACEFUNCTION();
    //    qDebug() << "Arg1: " << index << " Arg2:" << role;
    QVariant ret;
    if (role == Qt::DisplayRole) {
        SceneNode::shared_ptr p = NULL;
        if (index.isValid()) {
            p = _d->scene->findByIID(index.internalId());
        }
        if (p) {
            QString str = QString("%1 (%2)")
            .arg(QString(p->name().c_str()))
            .arg(p->iid());
            ret = str;
        } else {
            QString str = QString("Unamed");
            ret = str;
        }
    } else if (role == Qt::EditRole) {
        //        qDebug() << "data: EditRole";
    }
    //    qDebug() << ret;
    return ret;
}

bool DocumentModel::setData (const QModelIndex & index,
                        const QVariant & value, 
                        int role)
{
    TRACEFUNCTION();
    //    qDebug() << "Arg1: " << index << " Arg2:" << value << " Arg3:" << role;
    if (role == Qt::DisplayRole) {
        SceneNode::shared_ptr p = NULL;
        if (index.isValid()) {
            p = _d->scene->findByIID(index.internalId());
        }
        if (p) {
            p->setName(value.toString().toStdString());
            emit dataChanged(index, index);
            return true;
        }
        return false;
    } else if (role == Qt::EditRole) {
        SceneNode::shared_ptr p = NULL;
        if (index.isValid()) {
            p = _d->scene->findByIID(index.internalId());
        }
        if (p) {
            p->setName(value.toString().toStdString());
            emit dataChanged(index, index);
            return true;
        }
        return false;
    }
    return QAbstractItemModel::setData(index, value, role);
}

QModelIndex DocumentModel::index (int row, 
                             int column, 
                             const QModelIndex & parent ) const
{
    TRACEFUNCTION();
    //    qDebug() << "Arg1: " << row << " Arg2:" << column << " Arg3:" << parent;
    assert(_d && _d->scene);
    
    QModelIndex index;
    
    SceneNode::shared_ptr p = NULL;
    if (parent.isValid()) {
        p = _d->scene->findByIID(parent.internalId());
    }
    if (p) {
        p = p->item(row).lock();
    } else {
        p = _d->scene->item(row).lock();
    }
    if (p) {
        index = createIndex(row, 0, p->iid());
    }
    //    qDebug() << index;
    return index;
}

QModelIndex DocumentModel::parent ( const QModelIndex & index ) const
{
    TRACEFUNCTION();
    //    qDebug() << "Arg1: " << index;
    assert(_d && _d->scene);
    
    QModelIndex ret;
    if (!index.isValid()) {
        //        qDebug() << ret;
        return ret;
    }
    
    SceneNode::shared_ptr ptr = _d->scene->findByIID(index.internalId());
    SceneNode::shared_ptr parent, grandparent;
    int row = -1;
    
    // we need to obtaing the position of parent below its parent (i.e. the
    // grand parent of index)
    if (ptr) {
        parent = ptr->parent().lock();
        if (parent) {
            grandparent = parent->parent().lock();
            if (grandparent) {
                size_t n = 0;
                if (grandparent->itemIndex(parent, &n)) {
                    row = n;
                }
            }
        }
    } 
    if (parent && row > -1) {
        ret = createIndex(row, 0, parent->iid());
    }
    //    qDebug() << ret;
    return ret;
}

int DocumentModel::rowCount ( const QModelIndex & parent ) const
{
    TRACEFUNCTION();
    //    qDebug() << "Arg1: " << parent;
    
    assert(_d && _d->scene);
    int ret = 0;
    SceneNode::shared_ptr p;
    if (parent.isValid()) {
        p = _d->scene->findByIID(parent.internalId());
    } else {
        p = _d->scene;
    }
    if (p) {
        ret = p->count();
    }
    //    qDebug() << ret;
    return ret;
}
