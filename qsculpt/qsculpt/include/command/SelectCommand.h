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
#ifndef SELECTCOMMAND_H
#define SELECTCOMMAND_H

#include <QtCore/QVector>
#include "command/CommandBase.h"
#include "DocumentView.h"
#include "Point3D.h"
#include <QtCore/QPointer>

class TransformWidget;


#define SELECT_TYPE     "SELECTION_TYPE"

enum SelectionType{
    ST_Surface,
    ST_Vertex,
    ST_Edge,
    ST_Face
};

/**
 * Implements a simple object selection command.
 *
 * @author Juan Roberto Cabral Flores <roberto.cabral@gmail.com>
 */
class SelectCommand : public CommandBase
{
public:
    SelectCommand(ICommand* parent = 0);

    SelectCommand(const SelectCommand& cpy);

    virtual ~SelectCommand();

    // ICommand interface
    virtual ICommand* clone() const;
    virtual void execute();
    virtual void mouseMoveEvent(QMouseEvent* e);
    virtual void mousePressEvent(QMouseEvent* e);
    virtual void mouseReleaseEvent(QMouseEvent* e);
    virtual void paintGL(GlCanvas *c);
    virtual QWidget* getOptionsWidget();
    // End ICommand interface

    /**
     * This function set the kind of primitive that the selection should
     * work on.
     *
     * @param type primitive type to select. Can be any kind SelectionType
     */
    void setSelectionType(SelectionType type);

    /**
     * This function returns the kind of primitives that this selection works
     * on.
     */
    SelectionType selectionType();

private:
    ObjectContainer _objectsSelected;
    bool            _boxSelection;
    bool            _drawBox;
    Point3          _startPoint;
    Point3          _endPoint;
    Point3          _startPointWin;
    Point3          _endPointWin;
    SelectionType   _selectionType;

    void select();
    void selectSurface();
    void selectVertices();
    void selectFaces();
    void selectEdges();

    void unselectAll();

    static QPointer<TransformWidget>	_objectProperties;
};

#endif

