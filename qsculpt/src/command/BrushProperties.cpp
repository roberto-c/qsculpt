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
#include "command/BrushProperties.h"
#include "ui_BrushProperties.h"

BrushProperties::BrushProperties(QWidget* _parent)
: QWidget(_parent), ui_(new Ui::BrushProperties())
{
    ui_->setupUi(this);
}

BrushProperties::~BrushProperties()
{
    delete ui_;
}

double BrushProperties::getBrushRadius()
{
    return ui_->spinRadius->value();
}

void BrushProperties::setBrushRadius(double radius)
{
    if (radius > 0.00001)
        ui_->spinRadius->setValue(radius);
}

double BrushProperties::getBrushStrength()
{
    return ui_->spinStrength->value();
}

void BrushProperties::setBrushStrength(double strength)
{
    ui_->spinStrength->setValue(strength);
}

BrushCommand::BrushAction BrushProperties::getBrushAction()
{
    if (ui_->radPush->isChecked())
        return BrushCommand::Push;
    else if (ui_->radPull->isChecked())
        return BrushCommand::Pull;
    
    return BrushCommand::Push;
}

void BrushProperties::setBrushAction(BrushCommand::BrushAction action)
{
    switch(action)
    {
        case BrushCommand::Pull:
            ui_->radPull->setChecked(true);
            break;
        case BrushCommand::Push:
            ui_->radPush->setChecked(true);
            break;
    }
}

