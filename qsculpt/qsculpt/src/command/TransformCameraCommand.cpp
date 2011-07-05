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
#include "command/TransformCameraCommand.h"

TransformCameraCommand::TransformCameraCommand(): CommandBase()
{
}


TransformCameraCommand::~TransformCameraCommand()
{
}

void TransformCameraCommand::activate(bool /*active*/)
{
}

void TransformCameraCommand::execute()
{
    CommandBase::execute();
}

void TransformCameraCommand::mouseMoveEvent(QMouseEvent* e)
{
    CommandBase::mouseMoveEvent(e);
}

void TransformCameraCommand::mousePressEvent(QMouseEvent* e)
{
    CommandBase::mousePressEvent(e);
}

void TransformCameraCommand::mouseReleaseEvent(QMouseEvent* e)
{
    CommandBase::mouseReleaseEvent(e);
}

void TransformCameraCommand::undo()
{
    CommandBase::undo();
}

