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
#include <QtCore/QItemSelectionModel>
#include <QtGui/QStandardItem>
#include <QtGui/QStandardItemModel>

#include <PlastilinaCore/IDocument.h>
#include "DocumentModel.h"
#include "DocumentTreeWidget.h"
#include "DocumentView.h"
#include "QSculptApp.h"
#include "QSculptWindow.h"
#include "ui_DocumentTreeWidget.h"

static const QString TITLE("Document Tree");

struct DocumentTreeWidget::Private
{
    QScopedPointer<Ui::DocumentTreeWidget> ui;
    std::shared_ptr<DocumentModel>         doc;

    Private()
        : ui(new Ui::DocumentTreeWidget)
    {
    }
};

DocumentTreeWidget::DocumentTreeWidget(QWidget* parent)
    : QDockWidget(TITLE, parent)
    , _d(new Private)
{
    _d->ui->setupUi(this);
}

DocumentTreeWidget::~DocumentTreeWidget() {}

std::shared_ptr<DocumentModel> DocumentTreeWidget::document() const
{
    return _d->doc;
}

void DocumentTreeWidget::setDocument(
    const std::shared_ptr<DocumentModel>& doc)
{
    _d->doc = doc;
    updateTree();
}

QModelIndexList DocumentTreeWidget::selectedIndexes() const
{
    QItemSelectionModel* sm = _d->ui->nodeTree->selectionModel();
    if (!sm)
    {
        throw std::runtime_error(
            "DocumentTreeWidget::selectedIndexes: Invalid selectionModel");
    }
    return sm->selectedIndexes();
}

void DocumentTreeWidget::selectIndex(const QModelIndex& index)
{
    QItemSelectionModel* sm = _d->ui->nodeTree->selectionModel();
    if (!sm)
    {
        throw std::runtime_error(
            "DocumentTreeWidget::selectedIndexes: Invalid selectionModel");
    }
    sm->clearSelection();
    sm->select(index, QItemSelectionModel::Select);
}

void DocumentTreeWidget::itemActivated(const QModelIndex& index)
{
    qDebug() << "Index selected";
    //    SceneNode::shared_ptr ptr = _d->doc->findItem(index.internalId());
    //    DocumentView* view = g_pApp->getMainWindow()->getCurrentView();
    //    if (ptr && view) {
    //        ptr->setSelected(true);
    //        view->updateView();
    //    }
}

void DocumentTreeWidget::onSelectionChanged(const QItemSelection& selected,
                                            const QItemSelection& deselected)
{
    DocumentView*         view = g_pApp->getMainWindow()->getCurrentView();
    SceneNode::shared_ptr ptr;

    foreach (QModelIndex index, selected.indexes())
    {
        ptr = _d->doc->findItem(index.internalId());
        if (ptr)
        {
            ptr->setSelected(true);
        }
    }
    foreach (QModelIndex index, deselected.indexes())
    {
        ptr = _d->doc->findItem(index.internalId());
        if (ptr)
        {
            ptr->setSelected(false);
        }
    }
    if (view)
    {
        view->updateView();
    }
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
        QItemSelectionModel* sm = _d->ui->nodeTree->selectionModel();
        QObject::disconnect(sm, 0, this, 0);
        delete sm;
    }
    _d->ui->nodeTree->setModel(_d->doc.get());
    QItemSelectionModel* sm = _d->ui->nodeTree->selectionModel();
    QObject::connect(
        sm, SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)),
        this, SLOT(itemActivated(QModelIndex)));

    QObject::connect(sm,
                     SIGNAL(selectionChanged(const QItemSelection&,
                                             const QItemSelection&)),
                     this,
                     SLOT(onSelectionChanged(const QItemSelection&,
                                             const QItemSelection&)));
}
