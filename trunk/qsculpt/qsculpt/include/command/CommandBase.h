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
    IConfigContainer*   _configContainer;
    bool                _isActive;
    Point3              _intialPoint;
    Point3              _currentPoint;
    Point3              _finalPoint;
    Point3              _initialWinPoint;
    Point3              _currentWinPoint;
    Point3              _finalWinPoint;
    double              _modelMatrix[16];
    double              _projMatrix[16];
    GLint               _viewPort[4];
    bool                _rotationRadius;
private:
    Camera*             _currentCamera;
    Camera*             _intialCameraState;
    bool                _panViewMode;
    bool                _rotateViewMode;
};

#endif

