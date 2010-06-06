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
#ifndef COMMANDBASE_H
#define COMMANDBASE_H

#include <QtOpenGL>
#include "ICommand.h"
#include "Point3D.h"
#include "Camera.h"

/**
 * Base class for commands. Contains basic and common command implementation.
 *
 * @author Juan Roberto Cabral Flores <roberto.cabral@gmail.com>
*/
class CommandBase : public ICommand
{
public:
	/**
	 *
	 */
	CommandBase(ICommand* parent=0);

	/**
	 *
	 */
	CommandBase(const CommandBase&);

	/**
	 *
	 */
	CommandBase(const QString& text, ICommand* parent=0);

    virtual ~CommandBase();

    /*
     * ICommand interface implementation
     */
    virtual IConfigContainer& getConfig()const;
	virtual bool needsUserInteraction() const { return true; };
    virtual void activate(bool active);
    virtual bool isActive();
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);
    virtual void mouseMoveEvent(QMouseEvent *e);
    virtual void execute() {}
    virtual void paintGL();

protected:
    virtual void initializeConfigContainer(){}

protected:
    IConfigContainer*   m_configContainer;
    bool                m_isActive;
    Point3             m_intialPoint;
    Point3             m_currentPoint;
    Point3             m_finalPoint;
    Point3             m_initialWinPoint;
    Point3             m_currentWinPoint;
    Point3             m_finalWinPoint;
    double              m_modelMatrix[16];
    double              m_projMatrix[16];
    GLint               m_viewPort[4];
    bool                m_rotationRadius;
private:
    Camera*             m_currentCamera;
    Camera*             m_intialCameraState;
    bool                m_panViewMode;
    bool                m_rotateViewMode;
};

#endif

