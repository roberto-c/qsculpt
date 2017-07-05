//
//  DocumentModel.cpp
//  qsculpt
//
//  Created by Juan Cabral on 7/15/12.
//  Copyright (c) 2012 plastilinaware. All rights reserved.
//

#include "DocumentModel.h"
#include <iostream>

#include <PlastilinaCore/Document.h>
#include <PlastilinaCore/Logging.h>
#include <PlastilinaCore/Scene.h>
#include <PlastilinaCore/SceneNode.h>
#include <PlastilinaCore/Utilities.h>

using core::utils::convert_to;

#undef TRACEFUNCTION
#define TRACEFUNCTION()

struct DocumentModel::Impl
{
    IDocument::shared_ptr doc;
    Scene::shared_ptr     scene;

    Impl(const std::shared_ptr<IDocument>& doc)
        : doc(doc)
    {
        scene = doc->scene().lock();
    }
};

DocumentModel::DocumentModel(const std::shared_ptr<IDocument>& doc)
    : d(new Impl(doc))
{
}

DocumentModel::~DocumentModel() {}

SceneNode::shared_ptr DocumentModel::findItem(uint32_t iid) const
{
    return NULL;
}

QModelIndex DocumentModel::findItemIndex(uint32_t iid) const
{
    return QModelIndex();
}

int DocumentModel::columnCount(const QModelIndex& parent) const
{
    TRACEFUNCTION();
    //    qDebug() << "Args1: " << parent;
    //    qDebug() << 1;
    return 1;
}

Qt::ItemFlags DocumentModel::flags(const QModelIndex& index) const
{
    TRACEFUNCTION();
    // qDebug() << "Args1: " << index;
    // return QAbstractItemModel::flags(index);
    auto ret = QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
    // qDebug() << ret;
    return ret;
}

QVariant DocumentModel::data(const QModelIndex& index, int role) const
{
    TRACEFUNCTION();
    //    qDebug() << "Arg1: " << index << " Arg2:" << role;
    QVariant ret;
    if (role == Qt::DisplayRole)
    {
        SceneNode::shared_ptr p = NULL;
        if (index.isValid())
        {
            p = d->scene->findByIID(index.internalId());
        }
        if (p)
        {
            QString str = QString("%1 (%2)")
                              .arg(QString(p->name().c_str()))
                              .arg(p->iid());
            ret = str;
        }
        else
        {
            QString str = QString("Unamed");
            ret         = str;
        }
    }
    else if (role == Qt::EditRole)
    {
        //        qDebug() << "data: EditRole";
    }
    //    qDebug() << ret;
    return ret;
}

bool DocumentModel::setData(const QModelIndex& index, const QVariant& value,
                            int role)
{
    TRACEFUNCTION();
    //    qDebug() << "Arg1: " << index << " Arg2:" << value << " Arg3:" <<
    //    role;
    if (role == Qt::DisplayRole)
    {
        SceneNode::shared_ptr p = NULL;
        if (index.isValid())
        {
            p = d->scene->findByIID(index.internalId());
        }
        if (p)
        {
            p->setName(value.toString().toStdString());
            emit dataChanged(index, index);
            return true;
        }
        return false;
    }
    else if (role == Qt::EditRole)
    {
        SceneNode::shared_ptr p = NULL;
        if (index.isValid())
        {
            p = d->scene->findByIID(index.internalId());
        }
        if (p)
        {
            p->setName(value.toString().toStdString());
            emit dataChanged(index, index);
            return true;
        }
        return false;
    }
    return QAbstractItemModel::setData(index, value, role);
}

QModelIndex DocumentModel::index(int row, int column,
                                 const QModelIndex& parent) const
{
    TRACEFUNCTION();
    //    qDebug() << "Arg1: " << row << " Arg2:" << column << " Arg3:" <<
    //    parent;
    assert(d && d->scene);

    QModelIndex index;

    SceneNode::shared_ptr p = NULL;
    if (parent.isValid())
    {
        p = d->scene->findByIID(parent.internalId());
    }
    if (p)
    {
        p = p->item(row).lock();
    }
    else
    {
        p = d->scene->item(row).lock();
    }
    if (p)
    {
        index = createIndex(row, 0, p->iid());
    }
    //    qDebug() << index;
    return index;
}

QModelIndex DocumentModel::parent(const QModelIndex& index) const
{
    TRACEFUNCTION();
    //    qDebug() << "Arg1: " << index;
    assert(d && d->scene);

    QModelIndex ret;
    if (!index.isValid())
    {
        //        qDebug() << ret;
        return ret;
    }

    SceneNode::shared_ptr ptr = d->scene->findByIID(index.internalId());
    SceneNode::shared_ptr parent, grandparent;
    size_t                row = std::numeric_limits<size_t>::max();

    // we need to obtaing the position of parent below its parent (i.e. the
    // grand parent of index)
    if (ptr)
    {
        parent = ptr->parent().lock();
        if (parent)
        {
            grandparent = parent->parent().lock();
            if (grandparent)
            {
                size_t n = 0;
                if (grandparent->itemIndex(parent, &n))
                {
                    row = n;
                }
            }
        }
    }
    if (parent && row != std::numeric_limits<size_t>::max())
    {
        ret = createIndex(convert_to<int>(row), 0, parent->iid());
    }
    //    qDebug() << ret;
    return ret;
}

int DocumentModel::rowCount(const QModelIndex& parent) const
{
    TRACEFUNCTION();
    //    qDebug() << "Arg1: " << parent;

    assert(d && d->scene);
    int                   ret = 0;
    SceneNode::shared_ptr p;
    if (parent.isValid())
    {
        p = d->scene->findByIID(parent.internalId());
    }
    else
    {
        p = d->scene;
    }
    if (p)
    {
        convert_to(p->count(), ret);
    }
    //    qDebug() << ret;
    return ret;
}

void DocumentModel::addItem(const SceneNode::shared_ptr& node,
                            const QModelIndex&           parent)
{
    SceneNode::shared_ptr p;
    if (parent.isValid())
    {
        p = d->scene->findByIID(parent.internalId());
    }
    else
    {
        p = d->scene;
    }

    p->add(node);
}

bool DocumentModel::insertRow(int row, const QModelIndex& parent)
{
    return false;
}
