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


#ifndef QSCULPTWINDOW_H
#define QSCULPTWINDOW_H

#include <QMainWindow>
#include <QStack>
#include "ui_MainWindow.h"
#include "IDocument.h"
#include "CommandManager.h"

class QTextEdit;
class QDockWidget;
class DocumentView;
class ICommand;

/**
 * SolidPaint3D main window.
 */
class QSculptWindow : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT

public:
    QSculptWindow();
    virtual ~QSculptWindow();

    /**
     * Gets the current document. This document is the one that all commands
     * should operate on.
     */
    const IDocument* getCurrentDocument();

    /**
     * Get the current application command. This is the command that the
     * the user has selected to work with. This is used bu the document
     * editor to know what command the user has selected in order to trigger
     * the correct actions.
     *
     * @return current command
     */
    ICommand* getSelectedCommand() const;

    /**
     * Get the current view of the document.
     */
    DocumentView* getCurrentView() const;

    /**
     * Set the options configuration widget for the current command.
     */
    void setOptionsWidget( QWidget* widget);

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void newFile();
    void open();
    bool save();
    bool saveAs();
    void about();
    void documentWasModified();

    /**
     * Add a new box object to the document.
     */
    void addBox();

    /**
     * Add a new sphere object to the document.
     */
    void addSphere();

    /**
     * Show a grid as a helper guide.
     */
    void showGrid(bool val);

    /**
     * DocumentChanged signal handler. The object that was modified is not
     * used, so, it is not included on the slot signature.
     */
    void documentChanged(IDocument::ChangeType type);

    /**
     * This slot is called when a command is activated. The command activated
     * is identified by a string.
     */
    void commandActivated(QString commandName);

    /**
     * Set the application to fullscreen mode.
     */
    void viewFullscreen(bool value);

private:
    /**
     * Create the widgets used on ths window. It setups the UI, create the
     * dock widgets, create some of the connections of slot/signal
     */
    void createWidgets();

    void readSettings();
    void writeSettings();
    bool maybeSave();
    void loadFile(const QString &fileName);
    bool saveFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);
    QString strippedName(const QString &fullFileName);

    DocumentView*   m_glWidget;
	CommandManager	m_commandManager;
    QString         m_curFile;
    IDocument*      m_document;
    ICommand*       m_currentCommand;

    QActionGroup*   m_toolActionGroup;
    QDockWidget*    m_dockCommandOptions;
    QToolBar*       m_toolsToolbar;
};


#endif

