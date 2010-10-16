/***************************************************************************
 *   Copyright (C) 2010 by Juan Roberto Cabral Flores   *
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

#include "Stable.h"
#include "DocumentTreeWidget.h"
#include "IDocument.h"
#include <QStandardItemModel>
#include "ui_DocumentTreeWidget.h"
#include <QStandardItemModel>
#include <QStandardItem>
#include <QItemSelectionModel>

static const QString TITLE("Document Tree");

struct DocumentTreeWidget::Private
{
    QScopedPointer<Ui::DocumentTreeWidget> ui;
    QPointer<IDocument> doc;
    QStandardItemModel model;
    
    Private() : ui(new Ui::DocumentTreeWidget){}
};

DocumentTreeWidget::DocumentTreeWidget(QWidget* parent)
:   QDockWidget(TITLE, parent),
    _d(new Private)
{
    _d->ui->setupUi(this);
}

DocumentTreeWidget::~DocumentTreeWidget()
{
}

IDocument* DocumentTreeWidget::document() const
{
    return _d->doc;
}

void DocumentTreeWidget::setDocument(IDocument* doc)
{
    _d->doc = doc;
    updateTree();
}

void DocumentTreeWidget::itemActivated(const QModelIndex &index)
{
    qDebug() << "Index selected";
    
}

void DocumentTreeWidget::updateTree()
{
    if (!_d->doc)
    {
        return;
    }
    if (_d->ui->nodeTree->model()) 
    {
        //_d->ui->nodeTree->model()->
        QItemSelectionModel *sm = _d->ui->nodeTree->selectionModel();
        QObject::disconnect(sm, 0, this, 0);
    }
    _d->ui->nodeTree->setModel(&_d->model);
    QItemSelectionModel *sm = _d->ui->nodeTree->selectionModel();
    QObject::connect(sm, 
                     SIGNAL(currentChanged(const QModelIndex&,const QModelIndex&)),
                     this,
                     SLOT(itemActivated(QModelIndex)));
    
    QStandardItem *parentItem = _d->model.invisibleRootItem();
    parentItem->removeRows(0, parentItem->rowCount());
    parentItem->appendRow(reinterpret_cast<QStandardItem*>(_d->doc->rootNode()));
}
