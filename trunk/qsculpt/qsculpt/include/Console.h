/***************************************************************************
 *   Copyright (C) 2011 by Juan Roberto Cabral Flores   *
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

#ifndef CONSOLE_H
#define CONSOLE_H

#include <QString>

class ICommand;
class ConsoleWindow;

/**
  * Execute a given command entered by keyboard by using the ConsoleWindow.
  */
class Console
{
public:
    static Console* instance() ;

    /**
      * Destructor
      */
    virtual ~Console();

    /**
      * Register a command into the console. This makes the command available
      * to the console interpreter.
      */
    bool registerCommand(const QString& name, ICommand* cmd);

    /**
      * Unregister the command from the console.
      */
    bool unregisterCommand(const QString& name);

    /**
      * Execute the command
      *
      * @param command a text string stating the command to execute.
      *
      */
    bool evaluate(const QString& command);

    /**
      * Get the console window.
      */
    ConsoleWindow* consoleWindow();

    /**
     * Write a string into the console
     */
    void write(const QString &);
    
private:
    /**
      * Default constructor of the console.
      */
    Console();

    class Impl;
    QScopedPointer<Impl> _impl;
};


#define CONSOLE() Console::instance()

#endif // CONSOLE_H
