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

#include <QtGui/QMainWindow>
#include <QtCore/QStack>

#include <PlastilinaCore/IDocument.h>
#include "CommandManager.h"

class QTextEdit;
class QDockWidget;
class DocumentView;
class ICommand;
class Console;
class DocumentTreeWidget;

/**
 * SolidPaint3D main window.
 */
class QSculptWindow : public QMainWindow
{
    Q_OBJECT

public:
    QSculptWindow();
    virtual ~QSculptWindow();

    /**
     * Gets the current document. This document is the one that all commands
     * should operate on.
     */
    IDocument::const_shared_ptr getCurrentDocument() const;

    /**
     * Gets the current document. This document is the one that all commands
     * should operate on.
     * 
     * @return pointer to current document
     */
    IDocument::shared_ptr getCurrentDocument();

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

    /**
     * This method is used to get the pointer to one of the docking widget.
     *
     * The caller should cast the widget to the correct widget to use if needed.
     *
     * @param key id of the widget to retrieve. Each widget is added to a lookup
     * table. This parameter is the index in that lookup table.
     *
     * @return The widget registered with the given key. Null if the key is not
     * found. 
     */
    QWidget* toolWidget(const QString & key) const;
    
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

    struct Impl;
    QScopedPointer<Impl> d_;
};


#endif

