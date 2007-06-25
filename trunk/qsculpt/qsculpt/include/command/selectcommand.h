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
#ifndef SELECTCOMMAND_H
#define SELECTCOMMAND_H

#include <QVector>
#include "commandbase.h"
#include "documentview.h"
#include "point3d.h"

/**
 * Implements a simple object selection command.
 *
 * @author Juan Roberto Cabral Flores <roberto.cabral@gmail.com>
 */
class SelectCommand : public CommandBase
{
public:	
	SelectCommand(ICommand* parent = 0);
	
	SelectCommand(const SelectCommand& cpy);

    virtual ~SelectCommand();

	ICommand* clone() const;
	
    virtual void mouseMoveEvent(QMouseEvent* e);
    
    virtual void mousePressEvent(QMouseEvent* e);
    
    virtual void mouseReleaseEvent(QMouseEvent* e);

private:
    QVector<HitRecord> m_record;
    
    void selectObject();
};

#endif

