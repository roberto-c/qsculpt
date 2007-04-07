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
#include "transformwidget.h"
#include "qsculptapp.h"
#include "qsculptwindow.h"
#include "idocument.h"
#include "documentview.h"
#include "movecommand.h"
#include "iconfigcontainer.h"

TransformWidget::TransformWidget(QWidget* parent)
    : QWidget(parent), Ui::TransformWidget()
{
    setupUi(this);
    
    connect(m_apply, SIGNAL(clicked()), this, SLOT(applyTransform()));
}

TransformWidget::~TransformWidget()
{
}

void TransformWidget::enable(bool enable)
{
    m_apply->setEnabled(enable);
}

void TransformWidget::applyTransform()
{
    TransformCommand cmd;
    
    IConfigContainer& conf = cmd.getConfig();
    
    conf.setInt( CONF_ACTION, TransformCommand::AllActions);
    conf.setDouble( CONF_MOVE_X, m_posX->value());
    conf.setDouble( CONF_MOVE_Y, m_posY->value());
    conf.setDouble( CONF_MOVE_Z, m_posZ->value());
    conf.setDouble( CONF_ROTATE_X, m_rotX->value());
    conf.setDouble( CONF_ROTATE_Y, m_rotY->value());
    conf.setDouble( CONF_ROTATE_Z, m_rotZ->value());
    conf.setDouble( CONF_SCALE_X, 1);
    conf.setDouble( CONF_SCALE_Y, 1);
    conf.setDouble( CONF_SCALE_Z, 1);
    
    cmd.execute();
    
    SPAPP->getMainWindow()->getCurrentView()->updateView();
}

