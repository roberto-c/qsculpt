/***************************************************************************
*   Copyright (C) 2007 by Juan Roberto Cabral Flores   *
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
#include "stable.h"
#include "commandmanager.h"

#include "icommand.h"
#include <QAction>


CommandManager::CommandManager()
: m_currentCommand(NULL),
    m_undoAction(NULL),
    m_redoAction(NULL)
{
    m_undoStack = new QUndoStack;
    m_undoStack->setActive(true);
}

CommandManager::~CommandManager()
{
    delete m_undoStack;
}
	
bool CommandManager::registerCommand(QString name, QAction* action, ICommand* cmd)
{
	if (m_commandTable.contains(name))
		return false;
	
	m_commandTable.insert(name, cmd);
	m_actionTable.insert(name, action);
	connect(action, SIGNAL(triggered()), this, SLOT(commandTriggered()));
	
	return true;
}

void CommandManager::unregisterCommand(QString name)
{
	m_commandTable.remove(name);
	m_actionTable.remove(name);
}

QAction* CommandManager::createUndoAction(QObject * parent,
                          const QString & prefix ) const
{
    if (m_undoAction == NULL)
    {
        qDebug() << "QUndoStack::createUndoAction";
        m_undoAction = m_undoStack->createUndoAction(parent, prefix);
    }
    return m_undoAction;
}

QAction* CommandManager::createRedoAction( QObject * parent,
                           const QString & prefix ) const
{
    if (m_redoAction == NULL)
    {
        qDebug() << "QUndoStack::createRedoAction"; 
        m_redoAction = m_undoStack->createRedoAction(parent, prefix);
    }
    return m_redoAction;
}

void CommandManager::setActiveCommand(QString name)
{
	QAction* action = NULL;
	action = m_actionTable[name];
	if (action)
	{
		action->activate(QAction::Trigger);
	}
}

QString CommandManager::getActiveCommandName()
{
	return QString();
}

void CommandManager::commandTriggered()
{
	QAction* obj = qobject_cast<QAction*>(sender());
    if (obj)
    {
        ICommand* cmd = NULL;
		cmd = m_commandTable[m_actionTable.key(obj)];
		if (cmd)
		{
			ICommand* newCommand = cmd->clone();
			if (newCommand)
			{
				if (m_currentCommand)
					m_currentCommand->activate(false);
			
				m_currentCommand = newCommand;
				
				disconnect(m_currentCommand, SIGNAL(executed()), 0, 0);
				connect(m_currentCommand, SIGNAL(executed()),
						this, SLOT(commandExecuted()));
				
				m_currentCommand->activate(true);
			}
		}
	}
}

void CommandManager::commandExecuted()
{
	ICommand* cmd = qobject_cast<ICommand*>(sender());
    if (cmd)
    {
		ICommand* newCommand = cmd->clone();
		if (newCommand)
		{
			qDebug() << "Added command to undo stack";
            m_undoStack->push(cmd);
            if (m_currentCommand)
                m_currentCommand->activate(false);
            
            disconnect(m_currentCommand, SIGNAL(executed()), 0, 0);
            
            m_currentCommand = newCommand;
            connect(m_currentCommand, SIGNAL(executed()),
                    this, SLOT(commandExecuted()));
            
            m_currentCommand->activate(true);
		}
	}
}
