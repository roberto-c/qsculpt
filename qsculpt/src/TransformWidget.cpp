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
#include "TransformWidget.h"
#include <PlastilinaCore/IDocument.h>
#include "DocumentView.h"
#include "IConfigContainer.h"
#include "MoveCommand.h"
#include "QSculptApp.h"
#include "QSculptWindow.h"
#include "ui_TransformWidget.h"

TransformWidget::TransformWidget(QWidget* parent)
    : QWidget(parent)
    , ui_(new Ui::TransformWidget())
{
    ui_->setupUi(this);

    // connect(m_apply, SIGNAL(clicked()), this, SLOT(applyTransform()));
    qDebug() << "TransformWidget constructor";
}

TransformWidget::~TransformWidget()
{
    qDebug() << "TransformWidget destructor";
    delete ui_;
}

void TransformWidget::enable(bool enable)
{
    Q_UNUSED(enable);
    // m_apply->setEnabled(enable);
}

void TransformWidget::applyTransform()
{
    assert(ui_ != nullptr);
    TransformCommand cmd;

    IConfigContainer& conf = cmd.getConfig();

    conf.setInt(CONF_ACTION, TransformCommand::AllActions);
    conf.setDouble(CONF_MOVE_X, ui_->m_posX->value());
    conf.setDouble(CONF_MOVE_Y, ui_->m_posY->value());
    conf.setDouble(CONF_MOVE_Z, ui_->m_posZ->value());
    conf.setDouble(CONF_ROTATE_X, ui_->m_rotX->value());
    conf.setDouble(CONF_ROTATE_Y, ui_->m_rotY->value());
    conf.setDouble(CONF_ROTATE_Z, ui_->m_rotZ->value());
    conf.setDouble(CONF_SCALE_X, 1);
    conf.setDouble(CONF_SCALE_Y, 1);
    conf.setDouble(CONF_SCALE_Z, 1);

    cmd.execute();

    g_pApp->getMainWindow()->getCurrentView()->updateView();
}
