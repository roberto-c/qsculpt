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

#include <QVector>
#include <QHash>
#include <QMap>
#include "CommandBase.h"
#include "DocumentView.h"
#include "Point3D.h"

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
	typedef QMap<IObject3D*, QHash<int, Point3D> > DirtyFaceMap;

	void applyOperation(); 
	
	void selectObject();

    ObjectContainer		m_selectedObjects;
    IObject3D*          m_object;
    double              m_radius;
    double              m_depth;
    BrushAction         m_action;
    QVector<int>        m_vertexSelected;
    BrushProperties*    m_propertiesWindow;
	DirtyFaceMap		m_previousState;
	bool				m_undoCalled;
	static QImage		m_cursorImage;
	int					m_direction;
};

#endif

