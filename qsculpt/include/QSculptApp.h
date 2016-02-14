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
#ifndef QSCULPTAPP_H
#define QSCULPTAPP_H

#include <QtWidgets/QApplication>
#include <PlastilinaCore/IDocument.h>

#define g_pApp g_pApp

class QSculptWindow;

/**
 * Contains methods or variables that are used in all the application.
 * 
 * @author Juan Roberto Cabral Flores <roberto.cabral@gmail.com>
*/
class QSculptApp : public QApplication
{
public:
    /**
     * Application's constructor that receives arguments. Constructs an instance of
     * the main window.
     */
    QSculptApp(int& argc, char** argv);
    
    /**
     * Default detructor of the application
     */
    virtual ~QSculptApp();

    /**
     * Get the main widget of the application. The main widget generally is the
     * main window.
     * 
     * @return a pointer to the main widget
     */
    QSculptWindow* getMainWindow();
    
    /**
     * Get the current document.
     *
     * This function is used to retreive the document where the operations
     * are being executed on.
     */
    IDocument::shared_ptr getCurrentDocument();
    
    virtual bool	notify ( QObject * receiver, QEvent * e );

private:
    QSculptWindow* m_mainWindow; /**< main widget of the application. */
};

extern QSculptApp* g_pApp;

#endif

