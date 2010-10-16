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
#include <QtGui>
#include <QtOpenGL>
#include <QDockWidget>
#include <QFile>
#include <QTextStream>

#include "QSculptWindow.h"
#include "DocumentView.h"
#include "Document.h"
#include "MoveCommand.h"
#include "TransformWidget.h"
#include "SelectCommand.h"
#include "BrushCommand.h"
#include "SubdivideCommand.h"
#include "orbitcommand.h"
#include "ISurface.h"
#include "Console.h"
#include "ConsoleWindow.h"
#include "IConfigContainer.h"
#include "DocumentTreeWidget.h"

QSculptWindow::QSculptWindow()
    : m_documentView(new DocumentView(this)),
    m_document(new Document),
    m_currentCommand(NULL),
    m_toolActionGroup(NULL),
    m_dockCommandOptions(NULL),
    m_toolsToolbar(NULL)
{
    Q_CHECK_PTR(m_documentView);
    Q_CHECK_PTR(m_document);

    createWidgets();

    readSettings();
}

QSculptWindow::~QSculptWindow()
{
    delete m_document;
}

void QSculptWindow::createWidgets()
{
    setupUi(this);
    m_documentView->setDocument(m_document);

    setCentralWidget(m_documentView);

    m_dockCommandOptions = new QDockWidget("Options", NULL);
    Q_CHECK_PTR(m_dockCommandOptions);
    m_dockCommandOptions->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    addDockWidget(Qt::RightDockWidgetArea, m_dockCommandOptions);

    m_viewFullscreen->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_F));
    connect(m_showGrid, SIGNAL(toggled(bool)), m_documentView, SLOT(setGridVisible(bool)));
    connect(m_showNormals, SIGNAL(toggled(bool)), m_documentView, SLOT(setNormalsVisible(bool)));
    connect(m_viewFullscreen, SIGNAL(toggled(bool)), this, SLOT(viewFullscreen(bool)));
    connect(m_document, SIGNAL(changed(IDocument::ChangeType, ISurface*)), this, SLOT(documentChanged(IDocument::ChangeType)));

    connect(m_addBox, SIGNAL(activated()), this, SLOT(addBox()));
    connect(m_addSphere, SIGNAL(activated()), this, SLOT(addSphere()));

    _console = Console::instance();
    
    _docTree = new DocumentTreeWidget(this);
    _docTree->setDocument(m_document);

    QAction *action = m_commandManager.createUndoAction(this);
    Q_CHECK_PTR(action);
    action->setShortcut(QKeySequence::Undo);
    menuEdit->addAction(action);
    action = m_commandManager.createRedoAction(this);
    Q_CHECK_PTR(action);
    action->setShortcut(QKeySequence::Redo);
    menuEdit->addAction(action);

    m_toolActionGroup = new QActionGroup(this);
    Q_CHECK_PTR(m_toolActionGroup);
    m_toolsToolbar = addToolBar("Tools");

    ICommand* cmd = NULL;
    action = new QAction("Select", this);
    cmd = new SelectCommand;
    Q_CHECK_PTR(action);
    Q_CHECK_PTR(cmd);

    action->setToolTip("Select an object.");
    action->setCheckable(true);
    action->setIcon(QIcon(":/img_select.png"));
    menuTools->addAction(action);
    m_toolActionGroup->addAction(action);
    m_toolsToolbar->addAction(action);
    m_commandManager.registerCommand("Select", action, cmd);

    action = new QAction("SelectFaces", this);
    cmd = new SelectCommand;
    cmd->getConfig().setInt(SELECT_TYPE, ST_Face);
    action->setToolTip("Select faces.");
    action->setCheckable(true);
    action->setIcon(QIcon(":/img_select.png"));
    menuTools->addAction(action);
    m_toolActionGroup->addAction(action);
    m_toolsToolbar->addAction(action);
    m_commandManager.registerCommand("SelectFaces", action, cmd);

    action = new QAction("SelectVertices", this);
    cmd = new SelectCommand;
    cmd->getConfig().setInt(SELECT_TYPE, ST_Vertex);
    action->setToolTip("Select vertices.");
    action->setCheckable(true);
    action->setIcon(QIcon(":/img_select.png"));
    menuTools->addAction(action);
    m_toolActionGroup->addAction(action);
    m_toolsToolbar->addAction(action);
    m_commandManager.registerCommand("SelectVertices", action, cmd);

    cmd = new OrbitCommand;
    action = new QAction("Orbit", this);
    action->setCheckable(true);
    action->setToolTip("Orbit view");
    menuTools->addAction(action);
    m_toolActionGroup->addAction(action);
    m_toolsToolbar->addAction(action);
    m_commandManager.registerCommand("Orbit", action, cmd);

    //	action = new QAction("Transform", this);
    //	cmd = new TransformCommand;
    //	Q_CHECK_PTR(action);
    //	Q_CHECK_PTR(cmd);
    //	action->setToolTip("Move, rotate or scale an object.");
    //    action->setCheckable(true);
    //	//action->setEnabled(false);
    //	menuTools->addAction(action);
    //	m_toolActionGroup->addAction(action);
    //	m_toolsToolbar->addAction(action);
    //	m_commandManager.registerCommand("Transform", action, cmd);

    action = new QAction("Brush", this);
    cmd = new BrushCommand;
    Q_CHECK_PTR(action);
    Q_CHECK_PTR(cmd);
    action->setText("Brush");
    action->setToolTip("Deform the object using different kinds of brushes.");
    action->setCheckable(true);
    action->setIcon(QIcon(":/img_brush.png"));
    menuTools->addAction(action);
    m_toolActionGroup->addAction(action);
    m_toolsToolbar->addAction(action);
    m_commandManager.registerCommand("Brush", action, cmd);

    menuTools->addSeparator();
    m_toolsToolbar->addSeparator();

    action = new QAction("Subdivide", this);
    cmd = new SubdivideCommand;
    Q_CHECK_PTR(action);
    Q_CHECK_PTR(cmd);
    action->setToolTip("Subdivides each object face.");
    action->setIcon(QIcon(":/img_subdivide.png"));
    menuTools->addAction(action);
    m_toolsToolbar->addAction(action);
    m_commandManager.registerCommand("Subdivide", action, cmd);

    connect(&m_commandManager, SIGNAL(commandActivated(QString)),
            this, SLOT(commandActivated(QString)));

    // Activate default tool
    m_commandManager.setActiveCommand("Select");

    connect(m_save, SIGNAL(activated()), this, SLOT(save()));
    connect(m_saveAs, SIGNAL(activated()), this, SLOT(saveAs()));
    connect(m_open, SIGNAL(activated()), this, SLOT(open()));

    if (layout())
    {
    	layout()->setContentsMargins(0, 0, 0, 0);
    	layout()->setSpacing(0);
    }

    _console->consoleWindow()->show();
    
    _docTree->setFloating(true);
    _docTree->show();
}

const IDocument* QSculptWindow::getCurrentDocument()
{
    return m_document;
}

DocumentView* QSculptWindow::getCurrentView() const
{
    return m_documentView;
}

void QSculptWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        writeSettings();
        _console->consoleWindow()->close();
        event->accept();
    } else {
        event->ignore();
    }
}


void QSculptWindow::newFile()
{
    if (maybeSave()) {
        //textEdit->clear();
        setCurrentFile("");
    }
}

void QSculptWindow::open()
{
    if (maybeSave()) {
        QString fileName = QFileDialog::getOpenFileName(this);
        if (!fileName.isEmpty())
            loadFile(fileName);
    }
}

bool QSculptWindow::save()
{
    if (m_curFile.isEmpty()) {
        return saveAs();
    } else {
        return saveFile(m_curFile);
    }
}

bool QSculptWindow::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this);
    if (fileName.isEmpty())
        return false;

    return saveFile(fileName);
}

void QSculptWindow::about()
{
    QMessageBox::about(this, tr("About Application"),
                       tr("The <b>Application</b> example demonstrates how to "
                          "write modern GUI applications using Qt, with a menu bar, "
                          "toolbars, and a status bar."));
}

void QSculptWindow::documentWasModified()
{
}

void QSculptWindow::readSettings()
{
    QSettings settings("Trolltech", "Application Example");
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(400, 400)).toSize();
    resize(size);
    move(pos);
}

void QSculptWindow::writeSettings()
{
    QSettings settings("Trolltech", "Application Example");
    settings.setValue("pos", pos());
    settings.setValue("size", size());
}

bool QSculptWindow::maybeSave()
{
    /*
    if (textEdit->document()->isModified()) {
        int ret = QMessageBox::warning(this, tr("Application"),
                      tr("The document has been modified.\n"
                        "Do you want to save your changes?"),
                      QMessageBox::Yes | QMessageBox::Default,
                      QMessageBox::No,
                      QMessageBox::Cancel | QMessageBox::Escape);
        if (ret == QMessageBox::Yes)
            return save();
        else if (ret == QMessageBox::Cancel)
            return false;
    }
    */
    return true;
}

void QSculptWindow::loadFile(const QString &fileName)
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    if (m_document)
    {
        m_document->loadFile( fileName );
    }
    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File loaded"), 2000);
}

bool QSculptWindow::saveFile(const QString &fileName)
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    if (m_document)
    {
        m_document->saveFile( fileName );
    }
    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File saved"), 2000);
    return true;
}

void QSculptWindow::setCurrentFile(const QString &fileName)
{
    m_curFile = fileName;
    setWindowModified(false);

    QString shownName;
    if (m_curFile.isEmpty())
        shownName = "untitled.txt";
    else
        shownName = strippedName(m_curFile);

    setWindowTitle(tr("%1[*] - %2").arg(shownName).arg(tr("Application")));
}

QString QSculptWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

void QSculptWindow::addBox()
{
    m_document->addObject( IDocument::Box );
}

void QSculptWindow::addSphere()
{
    m_document->addObject( IDocument::Sphere );
    //	QList<IObject3D*> objects = m_document->getSelectedObjects();
    //	if (objects.size() > 0)
    //	{
    //		IObject3D* mesh = objects[0];
    //		int numVertices = mesh->getPointList().size();
    //		for (int i = 0; i < numVertices; ++i)
    //		{
    //			qDebug("Vertex %s - Normal %s",
    //				   qPrintable(mesh->getPointList().at(i).toString()),
    //				   qPrintable(mesh->getNormalList().at(i).toString())
    //				   );
    //		}
    //		int numFaces = mesh->getFaceList().size();
    //		for (int i = 0; i < numFaces; ++i)
    //		{
    //			Face f = mesh->getFaceList().at(i);
    //			qDebug("Face Index %d (%d, %d, %d, %d)", i, f.point[0],
    //				   f.point[1], f.point[2], f.point[3]);
    //		}
    //	}
}

void QSculptWindow::showGrid(bool val)
{
    m_documentView->setGridVisible( val );
}

ICommand* QSculptWindow::getSelectedCommand() const
{
    return m_commandManager.getActiveCommand();
}

void QSculptWindow::documentChanged(IDocument::ChangeType /*type*/)
{
}

void QSculptWindow::setOptionsWidget(QWidget* widget)
{
    qDebug() << "QSculptWindow::setOptionsWidget";

    if (widget)
    {
        // Get the current widget on the dock and hide it
        QWidget* wid = m_dockCommandOptions->widget();
        if (wid)
        {
            wid->hide();
        }
        // Set the new widget on the dock and be sure
        // it's visible
        m_dockCommandOptions->setWidget(widget);
        if (widget)
        {
            widget->show();
            m_dockCommandOptions->show();
        }
    }
}

void QSculptWindow::commandActivated(QString name)
{
    qDebug() << "QSculptWindow::commandActivated";
    Q_UNUSED(name);
    ICommand* cmd = m_commandManager.getActiveCommand();
    if (cmd)
    {

        setOptionsWidget(cmd->getOptionsWidget());
    }
}

void QSculptWindow::viewFullscreen(bool value)
{
    if (value)
    {
        showFullScreen();
        if (m_dockCommandOptions)
        {
            m_dockCommandOptions->setWindowOpacity(0.75);
            m_documentView->grabMouse(true);
        }
    }
    else
    {
        showNormal();
        if (m_dockCommandOptions)
        {
            m_dockCommandOptions->setWindowOpacity(1.0);
            m_documentView->grabMouse(false);
        }
    }
}
