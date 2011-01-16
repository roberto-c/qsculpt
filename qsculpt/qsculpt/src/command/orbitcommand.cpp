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
#include "Stable.h"
#include "orbitcommand.h"

struct OrbitCommand::Impl
{
    
};

OrbitCommand::OrbitCommand(ICommand *parent)
    : CommandBase(parent)
{
}

OrbitCommand::OrbitCommand(const OrbitCommand &cpy)
    : CommandBase(cpy)
{
}

OrbitCommand::~OrbitCommand()
{
}

ICommand* OrbitCommand::clone() const
{
    return new OrbitCommand(*this);
}

QWidget* OrbitCommand::getOptionsWidget() {
    return NULL;
}

void OrbitCommand::mousePressEvent(QMouseEvent *e)
{
}

void OrbitCommand::mouseReleaseEvent(QMouseEvent *e)
{
}

void OrbitCommand::mouseMoveEvent(QMouseEvent *e)
{
}

void OrbitCommand::paintGL(GlCanvas *c)
{
}