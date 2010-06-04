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
#include "SubdivideCommand.h"
#include <QtDebug>
#include <QThread>
#include <QProgressDialog>
#include "IDocument.h"
#include "IObject3D.h"
#include "QSculptApp.h"
#include "QSculptWindow.h"
#include "DocumentView.h"

SubdivideCommand::SubdivideCommand()
 : CommandBase("Subdivide")
{
}

SubdivideCommand::SubdivideCommand(const SubdivideCommand& cpy)
: CommandBase(cpy)
{
}

SubdivideCommand::~SubdivideCommand()
{
}

ICommand* SubdivideCommand::clone() const
{
	return new SubdivideCommand(*this);
}

void SubdivideCommand::undo()
{
}

void SubdivideCommand::redo()
{
}

void SubdivideCommand::execute()
{
	//if (activate == false)
	//	return;

    qDebug("execute SubdivideCommand()");
    QProgressDialog dlg("Subdividing the selected object...", 0, 0, 100,
						g_pApp->getMainWindow());
    dlg.setWindowModality(Qt::WindowModal);
    dlg.setAutoReset(true);
    dlg.setAutoClose(true);
    dlg.setValue(0);

//    QThread::connect(m_workerThread, SIGNAL(progress(int)),
//					 &dlg, SLOT(setValue(int)));

    dlg.show();

    const IDocument* doc = g_pApp->getMainWindow()->getCurrentDocument();

    if (!doc)
        return;
    
    qDebug() << "Start time: " <<QDateTime::currentDateTime();
    
    QList<IObject3D*> list = doc->getSelectedObjects();
    IObject3D* obj = list.at(0);
    if (obj) {
        qDebug() << "Object found";
    }
    qDebug() << "End time:" << QDateTime::currentDateTime();
    dlg.setValue(100);
    g_pApp->getMainWindow()->getCurrentView()->updateView();
}

