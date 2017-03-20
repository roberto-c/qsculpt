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

#ifndef COMMANDMANAGER_H
#define COMMANDMANAGER_H

#include <QtCore/QHash>
#include <QtCore/QObject>
#include <QtCore/QStack>
#include <QtCore/QString>
#include <QtWidgets/QUndoStack>

class ICommand;
class QAction;

/**
 * Manage the registration of new commands on the application.
 * Each command is mapped to an ID (a string).
 */
class CommandManager : public QObject
{
    Q_OBJECT

  public:
    /**
     * Default constructor
     */
    CommandManager();
    virtual ~CommandManager();

    bool registerCommand(QString name, QAction* action, ICommand* cmd);
    void unregisterCommand(QString name);

    void setActiveCommand(QString name);
    ICommand* getActiveCommand() const { return m_currentCommand; }
    QString   getActiveCommandName();

    QAction* createUndoAction(QObject*       parent,
                              const QString& prefix = QString()) const;

    QAction* createRedoAction(QObject*       parent,
                              const QString& prefix = QString()) const;
  signals:
    void commandExecuted(QString name);

    /**
     * This signal is sent when a new command is activated. This signal can be
     * used to know when a command has become active.
     */
    void commandActivated(QString name);

  private slots:
    void commandTriggered();

    void commandExecuted();

  private:
    ICommand* m_currentCommand;
    QHash<QString, ICommand*> m_commandTable;
    QHash<QString, QAction*>  m_actionTable;
    QUndoStack*      m_undoStack;
    mutable QAction* m_undoAction;
    mutable QAction* m_redoAction;
};

#endif
