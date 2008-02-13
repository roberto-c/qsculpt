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
#ifndef ICOMMAND_H
#define ICOMMAND_H

#include <QObject>
#include <QUndoCommand>

class IConfigContainer;
class QMouseEvent;

/**
 * Command interface. Every application command should implement this
 * interface.
 *
 * The command can have several configuration parameters.
 *
 * @author Juan Roberto Cabral Flores <roberto.cabral@gmail.com>
*/
class ICommand : public QObject, public QUndoCommand
{
    Q_OBJECT

public:
    /**
     * Default contructor.
     */
	ICommand(ICommand* parent=0) : QUndoCommand(parent) {
	}

	ICommand(const QString& text, ICommand* parent=0)
		: QUndoCommand(text, parent) {
	}

	ICommand(const ICommand& cpy) : QObject(cpy.parent()), QUndoCommand(){}
    /**
     * Default destructor.
     */
    virtual ~ICommand(){}

	/**
	 * Creates a new instance of the command. The new command instance should
	 * have, at least, the same configuration parameters as the original one.
	 */
	virtual ICommand* clone() const = 0;

	/**
	 * This method returns the options or configuration widget of the command.
	 * The widget returned should contain all the controls necesary to allow
	 * the user to change configuration parameters of the command.
	 */
	virtual QWidget* getOptionsWidget() = 0;

    /**
     * Set the command as the active one. This is called with the
     * parameter in true to indicate that the command is active and should
     * be prepared to accept the input from the user (like mouse events, etc).
     *
     * To set the command as a non active, then pass false in
     * the active parameter.
     *
     * @param active True, the command is active. False, the command
     * is no longer active.
     */
    virtual void activate(bool active) = 0;

    /**
     * Indicate if the command is active or incactive. An active command is a
     * command that is in execution.
     */
    virtual bool isActive() = 0;

    /**
     * Returns a reference to the container of config paramters for the command
     *
     * @return referece to an IConfigContainer class
     */
    virtual IConfigContainer& getConfig()const = 0;

    /**
     * Called when a mouse press event ocurrs. This method is called by the
     * widget (a QGLWidget).
     *
     */
    virtual void mousePressEvent(QMouseEvent *e) = 0;

    /**
     * Called when a mouse release event ocurrs. This method is called by the
     * widget (a QGLWidget).
     *
     */
    virtual void mouseReleaseEvent(QMouseEvent *e) = 0;

    /**
     * Called when a mouse move event ocurrs. This method is called by the
     * widget (a QGLWidget).
     *
     */
    virtual void mouseMoveEvent(QMouseEvent *e) = 0;

public slots:
    /**
     * Execute the command in no interactive way. This means, that once that all the
     * parameters of the command are set, the command will be executed without user
     * intervention.
     */
    virtual void execute() = 0;


signals:
	/**
	 * Serves as a way to indicate when the command has finished its execution.
	 * This signal should be sent at the end of the undo() method implementation.
	 */
    void executed();
};

#endif

