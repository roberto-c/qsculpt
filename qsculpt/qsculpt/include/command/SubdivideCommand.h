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
#ifndef SUBDIVIDECOMMAND_H
#define SUBDIVIDECOMMAND_H

#include <QThread>
#include <utility>
#include "command/CommandBase.h"

class ISurface;
struct Face;
class Vertex;

/**
 * Subdivision command. Subdvide the selected object.
 *
 * @author Juan Roberto Cabral Flores <roberto.cabral@gmail.com>
 */
class SubdivideCommand : public CommandBase
{
    Q_OBJECT

public:
    SubdivideCommand();

    SubdivideCommand(const SubdivideCommand& cpy);

    ~SubdivideCommand();

    // ICommand Interface
    virtual ICommand* clone() const;
    virtual bool needsUserInteraction() const { return false; }
    virtual void execute();
    virtual void undo();
    virtual void redo();
    virtual QWidget* getOptionsWidget(){return NULL;}
    // End ICommand Interface

private:
    void subdivideFace(ISurface & obj, Face& f);

    class Impl;
    QScopedPointer<Impl> _d;

};

#endif

