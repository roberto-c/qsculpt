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
#include "BrushProperties.h"

BrushProperties::BrushProperties(QWidget* _parent)
: QWidget(_parent)
{
    setupUi(this);
}

BrushProperties::~BrushProperties()
{
}

double BrushProperties::getBrushRadius()
{
    return spinRadius->value();
}

void BrushProperties::setBrushRadius(double radius)
{
    if (radius > 0.00001)
        spinRadius->setValue(radius);
}

double BrushProperties::getBrushStrength()
{
    return spinStrength->value();
}

void BrushProperties::setBrushStrength(double strength)
{
    spinStrength->setValue(strength);
}

BrushCommand::BrushAction BrushProperties::getBrushAction()
{
    if (radPush->isChecked())
        return BrushCommand::Push;
    else if (radPull->isChecked())
        return BrushCommand::Pull;
    
    return BrushCommand::Push;
}

void BrushProperties::setBrushAction(BrushCommand::BrushAction action)
{
    switch(action)
    {
        case BrushCommand::Pull:
            radPull->setChecked(true);
            break;
        case BrushCommand::Push:
            radPush->setChecked(true);
            break;
    }
}

