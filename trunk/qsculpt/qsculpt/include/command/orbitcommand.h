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

#ifndef ORBITCOMMAND_H
#define ORBITCOMMAND_H

#include "CommandBase.h"

/**
 * Command to move around the view.
 *
 * Note: Should this be a capability of GLCanvas instead of a command?
 */
class OrbitCommand : public CommandBase
{
    class Impl;
    QScopedPointer<Impl> _d;
    
public:
    /**
     * Constructor for commands.
     */
    OrbitCommand(ICommand* parent = 0);

    /**
     * Copy constructor. Makes a copy of the command.
     */
    OrbitCommand(const OrbitCommand& cpy);

    virtual ~OrbitCommand();

    // ICommand interface
    /**
     * Makes a deep copy of the command.
     */
    virtual ICommand* clone() const;

    virtual QWidget* getOptionsWidget();
    
    /**
     *
     */
    virtual void mousePressEvent(QMouseEvent *e);
    
    /**
     * Called when a mouse release event ocurrs. This method is called by the
     * widget (a QGLWidget).
     *
     */
    virtual void mouseReleaseEvent(QMouseEvent *e);
    
    /**
     * Called when a mouse move event ocurrs. This method is called by the
     * widget (a QGLWidget).
     *
     */
    virtual void mouseMoveEvent(QMouseEvent *e);
    
    /**
     * Used to display anything specific to the command as user feedback.
     */
    virtual void paintGL(GlCanvas *c);
};

#endif // ORBITCOMMAND_H
