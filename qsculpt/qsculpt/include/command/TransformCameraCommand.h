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
#ifndef TRANSFORMCAMERACOMMAND_H
#define TRANSFORMCAMERACOMMAND_H

#include "CommandBase.h"
#include "Point3D.h"

/**
 * Moves and chage aim of camera.
 *
 * @author Juan Roberto Cabral Flores <roberto.cabral@gmail.com>
 */
class TransformCameraCommand : public CommandBase
{
public:
    TransformCameraCommand();

    ~TransformCameraCommand();

    // ICommand Interface
    virtual ICommand* clone() const {
    	return new TransformCameraCommand(*this);
    }
    virtual void activate(bool active);
    virtual void execute();
    virtual void mouseMoveEvent(QMouseEvent* e);
    virtual void mousePressEvent(QMouseEvent* e);
    virtual void mouseReleaseEvent(QMouseEvent* e);
    virtual void undo();
    virtual QWidget* getOptionsWidget(){return NULL;}
    // End ICommand Interface

private:
    Point3D     m_eyePosition;
    Point3D     m_targetDirection;
    Point3D     m_upVector;
};

#endif

