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
#ifndef BRUSHCOMMAND_H
#define BRUSHCOMMAND_H

#include <QtCore/QVector>
#include <QtCore/QHash>
#include <QtCore/QMap>
#include <PlastilinaCore/Point3D.h>
#include "command/CommandBase.h"
#include "DocumentView.h"


class BrushProperties;

/**
 * Implements a simple object selection command.
 *
 * @author Juan Roberto Cabral Flores <roberto.cabral@gmail.com>
 */
class BrushCommand : public CommandBase
{
public:
    enum BrushAction {
        Push,
        Pull
    };

    BrushCommand(ICommand* parent=0);

    BrushCommand(const BrushCommand& cpy);

    virtual ~BrushCommand();

    // ICommand Interface
    virtual ICommand* clone() const;
    virtual void activate(bool active);
    virtual void mouseMoveEvent(QMouseEvent* e);
    virtual void mousePressEvent(QMouseEvent* e);
    virtual void mouseReleaseEvent(QMouseEvent* e);
    virtual void undo();
    virtual void redo();
    virtual QWidget* getOptionsWidget();
    // End ICommand Interface

private:
    typedef QMap<ISurface*, QHash<int, Point3> > DirtyFaceMap;

    void applyOperation();

    void selectObject();

    ObjectContainer     _selectedObjects;
    ISurface*           _object;
    double              _radius;
    double              _depth;
    BrushAction         _action;
    std::vector<size_t> _vertexSelected;
    BrushProperties*    _propertiesWindow;
    DirtyFaceMap        _previousState;
    bool                _undoCalled;
    static QImage       _cursorImage;
    int                 _direction;
};

#endif

