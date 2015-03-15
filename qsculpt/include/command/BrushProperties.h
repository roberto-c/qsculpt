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
#ifndef BRUSHPROPERTIES_H
#define BRUSHPROPERTIES_H

#include <QtGui/QWidget>
#include "../ui/ui_BrushProperties.h"
#include "command/BrushCommand.h"

class BrushProperties : public QWidget, private Ui::BrushProperties
{
    Q_OBJECT
    
public:
    BrushProperties(QWidget* parent);
    virtual ~BrushProperties();
    
// public signals:
    double getBrushRadius();
    void setBrushRadius(double radius);
    
    double getBrushStrength();
    void setBrushStrength(double strength);
    
    BrushCommand::BrushAction getBrushAction();
    void setBrushAction(BrushCommand::BrushAction action);
};


#endif

