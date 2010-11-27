/***************************************************************************
 *   Copyright (C) $YEAR$ by Juan Roberto Cabral Flores   *
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
#include "Stable.h"
#include "Console.h"
#include "ConsoleWindow.h"

#include <QHash>
#include <string>
#include <iostream>
#include <QStringList>
#include "command/ICommand.h"
#include "command/IConfigContainer.h"

using namespace std;

typedef QHash< QString, ICommand* > CommandMap;


class Console::Impl
{
public:
    /**
      * Parses a command line and constructs a new command, ready to execute
      * the command.
      */
    bool parseCommandLine(const QString& line, ICommand** cmd);


    CommandMap commands; /*< list of command registered*/
    QScopedPointer<ConsoleWindow> window;
    QStringList buffer;
};

Console::Console()
    : _impl(new Impl)
{
    _impl->window.reset(new ConsoleWindow());
}

Console::~Console()
{
}

Console* Console::instance()
{
    static Console* console = NULL;
    if (!console) console = new Console();
    return console;
}

bool Console::registerCommand(const QString& name, ICommand* cmd)
{
    bool res = false;
    // add command to list of commands
    if (cmd)
    {
        _impl->commands[name] = cmd;
        res = true;
    }
    return res;
}

bool Console::unregisterCommand(const QString& name)
{
    _impl->commands.erase(_impl->commands.find(name));
    return true;
}

bool Console::evaluate(const QString& command)
{
    ICommand* cmd = NULL;
    _impl->parseCommandLine(command, &cmd);
    if (cmd)
    {
        write(command);
        cmd->execute();
        return true;
    }
    return false;
}

ConsoleWindow* Console::consoleWindow()
{
    return _impl->window.data();
}

void Console::write(const QString & str)
{
    _impl->buffer << str.split('\n');
    _impl->window.data()->write(str);
}

bool Console::Impl::parseCommandLine(const QString& line, ICommand** cmd)
{
    bool res = false;
    // just set the command to NULL. this is work in progress.
    if (cmd)
    {
        *cmd = NULL;

        // Extract the command name
        QStringList tokens = line.trimmed().split(" ");
        // Check if the command name is registered
        if (commands.find(tokens[0]) != commands.end())
        {
            *cmd = commands[tokens[0]]->clone();
            if (*cmd)
            {
                (*cmd)->config().setString("0", tokens[0]);
                for (int i = 1; i < tokens.size(); ++i) 
                {
                    (*cmd)->config().setString(QString::number(i), tokens[i]);
                }
                res = true;
            }
        }
    }
    return res;
}
