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

#include <map>
#include <string>
#include <iostream>
#include "command/ICommand.h"

using namespace std;

typedef map< string, ICommand* > CommandMap;


class Console::Impl
{
public:
    /**
      * Parses a command line and constructs a new command, ready to execute
      * the command.
      */
    bool parseCommandLine(std::string line, ICommand** cmd);


    CommandMap commands; /*< list of command registered*/
    ConsoleWindow *window;
};

Console::Console()
    : _impl(new Impl)
{
    _impl->window = new ConsoleWindow();
}

Console::~Console()
{
    delete _impl;
}

Console* Console::instance()
{
    static Console* console = NULL;
    if (!console) console = new Console();
    return console;
}

bool Console::registerCommand(const std::string& name, ICommand* cmd)
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

bool Console::unregisterCommand(const std::string& name)
{
    _impl->commands.erase(_impl->commands.find(name));
    return true;
}

bool Console::evaluate(const std::string& command)
{
    ICommand* cmd = NULL;
    _impl->parseCommandLine(command, &cmd);
    if (cmd)
    {
        cmd->execute();
    }
    return false;
}

ConsoleWindow* Console::consoleWindow()
{
    return _impl->window;
}

bool Console::Impl::parseCommandLine(std::string line, ICommand** cmd)
{
    // just set the command to NULL. this is work in progress.
    if (cmd) *cmd = NULL;
    return false;
}
