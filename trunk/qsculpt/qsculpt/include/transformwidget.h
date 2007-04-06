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
#ifndef TRANSFORMWIDGET_H
#define TRANSFORMWIDGET_H

#include "ui_transformWidget.h"
#include <QWidget>

class ICommand;
/**
 * Widget for object transformation like change position and change
 * orientation.
 *
 * @author Juan Roberto Cabral Flores <roberto.cabral@gmail.com>
 */
class TransformWidget : public QWidget, private Ui::TransformWidget
{
    Q_OBJECT

public:
    TransformWidget(QWidget* parent);

    virtual ~TransformWidget();

    void enable(bool enable);

private slots:
    void applyTransform();
};

#endif
