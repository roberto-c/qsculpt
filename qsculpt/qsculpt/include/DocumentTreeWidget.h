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

#ifndef DOCUMENTTREEWIDGET_H_
#define DOCUMENTTREEWIDGET_H_

#include <QtGui/QDockWidget>
#include <QtCore/QModelIndex>
#include "IDocument.h"

class DocumentTreeWidget : public QDockWidget
{
    Q_OBJECT
    Q_PROPERTY(IDocument::shared_ptr document READ document WRITE setDocument)
    
public:
    DocumentTreeWidget(QWidget* parent);
    virtual ~DocumentTreeWidget();
    
    /**
     * Gets the document to which this DocumentTree is associated to.
     */
    IDocument::shared_ptr document() const;
    
    /**
     * Sets the document to which this DocumentTree instance is associated to.
     */
    void setDocument(IDocument::shared_ptr doc);
    
public slots:
    void itemActivated(const QModelIndex &index);
    
    /**
     * Handles the selection / deselection of item in the model view.
     *
     */
    void onSelectionChanged(const QItemSelection & selected,
                            const QItemSelection & deselected );
    
private:
    void updateTree();
    
    struct Private;
    QScopedPointer<Private> _d;
    
};

#endif // DOCUMENTTREEWIDGET_H_ 

