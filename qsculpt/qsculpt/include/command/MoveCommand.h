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
#ifndef MOVECOMMAND_H
#define MOVECOMMAND_H

#include "CommandBase.h"
#include "Point3D.h"
#include <QList>
#include <QPoint>

class IObject3D;

// Configuration keys constants
#define CONF_ACTION         "ACTION"
#define CONF_MOVE_AXIS      "MOVE_ON_AXIS"
#define CONF_ROTATE_AXIS    "ROTATE_ON_AXIS"
#define CONF_SCALE_AXIS     "ROTATE_ON_AXIS"

#define CONF_MOVE_X         "MX"
#define CONF_MOVE_Y         "MY"
#define CONF_MOVE_Z         "MZ"
#define CONF_ROTATE_X       "RX"
#define CONF_ROTATE_Y       "RY"
#define CONF_ROTATE_Z       "RZ"
#define CONF_SCALE_X        "SX"
#define CONF_SCALE_Y        "SY"
#define CONF_SCALE_Z        "SZ"

/**
 * Command for transformation of an object. This command can move, rotate
 * or scale an object.
 *
 * @author Juan Roberto Cabral Flores <roberto.cabral@gmail.com>
 */
class TransformCommand : public CommandBase
{
public:
    /**
     * Action enumeration flags. Identifies the action of the command
     */
    enum Action {
        Move = 0,                   /**< Move action */
        Rotate,                     /**< Rotate action */
        Scale,                      /**< Scale action */
        AllActions                  /**< Do all actions */
    };

    /**
     * Enumeration flags used as the posible configuration values for
     * the axis movement
     */
    enum MoveAxis {
        XAxis = 0x00000001,         /**< Move on X axis */
        YAxis = 0x00000002,         /**< Move on X axis */
        ZAxis = 0x00000004,         /**< Move on X axis */
        XYAxis = XAxis | YAxis,     /**< Move on X and Y axis */
        XZAxis = XAxis | ZAxis,     /**< Move on X and Z axis */
        YZAxis = YAxis | ZAxis      /**< Move on Y and Z axis */
    }RotateAxis, ScaleAxis;

    /**
     * Move command constructor. Fill the configuration values container with
     * default values (ex. movement on XY axis as default).
     */
    TransformCommand();

    virtual ~TransformCommand();

    /**
     * Creates a copy of this object
     */
    virtual ICommand* clone() const {
    	return new TransformCommand(*this);
    }

    /**
     * Execute the transformation command.
     */
    virtual void execute();

    /**
     * Overriden from CommandBase. Mark the command as active/inactive.
     * If the active parameter is false and the object is moving, then
     * the command is canceled and the object is returned to their
     * initial position.
     *
     * @param active true to mark the command as active, false to
     * terminate the command.
     */
    virtual void activate(bool active);

    /**
     * Overriden from CommandBase. This stores the initial position of the
     * object.
     */
    virtual void mousePressEvent(QMouseEvent* e);

    /**
     * Overriden from CommandBase. Move the object acording to the mouse
     * movement. The movement is across the axis configurated.
     */
    virtual void mouseMoveEvent(QMouseEvent* e);

    /**
     * Overriden from CommandBase. Finish the movement of the object.
     */
    virtual void mouseReleaseEvent(QMouseEvent* e);

    /**
     * Overriden from ICommand. Returns the configuration widget for this
     * command.
     */
    virtual QWidget* getOptionsWidget() {
    	return NULL;
    }

private:
    bool                m_actionFinished;   /**< Flag to indicate that the command
                                             * is modifying the object. */
    Point3             m_initial;          /**< Initial position of the object. */
    Point3             m_delta;            /**< Change of position between mouse
                                             * movements */
    Point3             m_final;            /**< Final postion of the object. */
    QPoint              m_mousePosition;    /**< Initial mouse position */
    QList<IObject3D*>   m_objects;          /**< Object list to move */
};

#endif

