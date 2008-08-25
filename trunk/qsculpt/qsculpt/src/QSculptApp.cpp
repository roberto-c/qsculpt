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
#include "QSculptApp.h"
#include <QWidget>
#include "QSculptWindow.h"
#include "IDocument.h"


QSculptApp* g_pApp = NULL;

QSculptApp::QSculptApp(int& argc, char** argv)
    : QApplication(argc, argv),
    m_mainWindow(new QSculptWindow)
{
}

QSculptApp::~QSculptApp()
{
}

QSculptWindow* QSculptApp::getMainWindow()
{
    return m_mainWindow;
}


/**
 * Main entry point.
 *
 * Creates the application object. Passes the program arguments to the
 * applicatoin object to process them. Creates and show the main
 * application window.
 */
int main( int argc, char ** argv ) {
    QSculptApp a( argc, argv );
    
	g_pApp = (QSculptApp*)QSculptApp::instance();
	
	g_pApp->setOrganizationName("QSculpt");
	g_pApp->setOrganizationDomain("qsculpt.com");
	g_pApp->setApplicationName("QSculpt");
    g_pApp->getMainWindow()->show();
    
	int result = a.exec();

    return result;
}