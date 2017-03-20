//
//  DocumentModel.h
//  qsculpt
//
//  Created by Juan Cabral on 7/15/12.
//  Copyright (c) 2012 plastilinaware. All rights reserved.
//

#ifndef qsculpt_DocumentModel_h
#define qsculpt_DocumentModel_h

#include <QtCore/QAbstractItemModel>
#include <QtCore/QObject>

#include <PlastilinaCore/IDocument.h>
#include <PlastilinaCore/SceneNode.h>

class DocumentModel : public QAbstractItemModel,
                      public std::enable_shared_from_this<DocumentModel>
{
  public:
    DocumentModel(const std::shared_ptr<IDocument>& doc);

    virtual ~DocumentModel();

    /**
     * This method add a new item into the document.
     *
     * The item is an scene node and it is inserted under the parent stated by
     * the parent QModelIndex. If the index is invalid, then the node is
     * inserted at the root level.
     *
     * @param node Shared pointer to the node to insert. The implementation of
     * this class will share ownership of the node.
     *
     * @param parent QModelIndex which should be the parent. The node will be
     * inserted as a child of parent node. If parent is invalid, the node is
     * going to be inerted as child of the root node.
     */
    virtual void addItem(const SceneNode::shared_ptr& node,
                         const QModelIndex&           parent = QModelIndex());

    /**
     * Return the QModelIndex that corresponds to an SceneNode with ID iid.
     *
     * @param iid Instance ID of the node to look for.
     * @return QModelIndex representing the node.
     */
    virtual QModelIndex findItemIndex(uint32_t iid) const;

    /**
     *
     */
    virtual SceneNode::shared_ptr findItem(uint32_t iid) const;

    /**
     * Returns the number of columns for this model.
     *
     * From QAbstractItemModel
     */
    virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;

    virtual QVariant data(const QModelIndex& index,
                          int                role = Qt::DisplayRole) const;

    virtual QModelIndex
    index(int row, int column,
          const QModelIndex& parent = QModelIndex()) const;

    virtual QModelIndex parent(const QModelIndex& index) const;

    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;

    virtual bool insertRow(int                row,
                           const QModelIndex& parent = QModelIndex());

    virtual bool setData(const QModelIndex& index, const QVariant& value,
                         int role = Qt::EditRole);

    virtual Qt::ItemFlags flags(const QModelIndex& index) const;

  private:
    struct Impl;
    std::shared_ptr<Impl> d;
};

#endif
