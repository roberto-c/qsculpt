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
#include <QtGui/QtGui>
#include <QtOpenGL/QtOpenGL>
#include <QtGui/QDockWidget>
#include <QtCore/QFile>
#include <QtCore/QTextStream>

#include <PlastilinaCore/Document.h>
#include <PlastilinaCore/SceneNode.h>
#include <PlastilinaCore/subdivision/Box.h>
#include <PlastilinaCore/ISurface.h>
#include <PlastilinaCore/Material/PhongMaterial.h>
#include "QSculptWindow.h"
#include "DocumentView.h"
#include "MoveCommand.h"
#include "TransformWidget.h"
#include "SelectCommand.h"
#include "BrushCommand.h"
#include "SubdivideCommand.h"
#include "orbitcommand.h"
#include "Console.h"
#include "ConsoleWindow.h"
#include "IConfigContainer.h"
#include "DocumentTreeWidget.h"
#include "MeshEditCommands.h"
#include "ui_MainWindow.h"
#include "DocumentModel.h"

struct QSculptWindow::Impl : public Ui::MainWindow {
    DocumentView*               documentView;
    CommandManager              commandManager;
    QString                     curFile;
    IDocument::shared_ptr       document;
    ICommand*                   currentCommand;
    
    QActionGroup*               toolActionGroup;
    QDockWidget*                dockCommandOptions;
    QToolBar*                   toolsToolbar;
    Console*                    console;
    DocumentTreeWidget*         docTree;
    std::shared_ptr<DocumentModel> docModel;
};

QSculptWindow::QSculptWindow()
    : QMainWindow(), d_(new Impl)
{
    createWidgets();

    readSettings();
}

QSculptWindow::~QSculptWindow()
{
}

void QSculptWindow::createWidgets()
{
    d_->setupUi(this);
    d_->documentView = new DocumentView(this);
    

    setCentralWidget(d_->documentView);

    d_->dockCommandOptions = new QDockWidget("Options", NULL);
    Q_CHECK_PTR(d_->dockCommandOptions);
    d_->dockCommandOptions->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    addDockWidget(Qt::RightDockWidgetArea, d_->dockCommandOptions);

    d_->m_viewFullscreen->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_F));
    connect(d_->m_showGrid, SIGNAL(toggled(bool)), d_->documentView, SLOT(setGridVisible(bool)));
    connect(d_->m_showNormals, SIGNAL(toggled(bool)), d_->documentView, SLOT(setNormalsVisible(bool)));
    connect(d_->m_viewFullscreen, SIGNAL(toggled(bool)), this, SLOT(viewFullscreen(bool)));
    //connect(d_->document.get(), SIGNAL(changed(IDocument::ChangeType, ISurface*)),
    //        this, SLOT(documentChanged(IDocument::ChangeType)));

    d_->console = Console::instance();
    
    d_->docTree = new DocumentTreeWidget(this);
    d_->docTree->setDocument(d_->docModel);

    QAction *action = NULL;
    ICommand* cmd = NULL;

    //action = new QAction("AddSurface", this);
    cmd = new AddSurfaceCommand;
    d_->commandManager.registerCommand("AddBox", d_->m_addBox, cmd);
    
    action = d_->commandManager.createUndoAction(this);
    Q_CHECK_PTR(action);
    action->setShortcut(QKeySequence::Undo);
    d_->menuEdit->addAction(action);
    action = d_->commandManager.createRedoAction(this);
    Q_CHECK_PTR(action);
    action->setShortcut(QKeySequence::Redo);
    d_->menuEdit->addAction(action);

    d_->toolActionGroup = new QActionGroup(this);
    Q_CHECK_PTR(d_->toolActionGroup);
    d_->toolsToolbar = addToolBar("Tools");

    
    action = new QAction("Select", this);
    cmd = new SelectCommand;
    Q_CHECK_PTR(action);
    Q_CHECK_PTR(cmd);

    action->setToolTip("Select an object.");
    action->setCheckable(true);
    action->setIcon(QIcon(":/img_select.png"));
    d_->menuTools->addAction(action);
    d_->toolActionGroup->addAction(action);
    d_->toolsToolbar->addAction(action);
    d_->commandManager.registerCommand("Select", action, cmd);

    action = new QAction("SelectFaces", this);
    cmd = new SelectCommand;
    cmd->getConfig().setInt(SELECT_TYPE, ST_Face);
    action->setToolTip("Select faces.");
    action->setCheckable(true);
    action->setIcon(QIcon(":/img_select.png"));
    d_->menuTools->addAction(action);
    d_->toolActionGroup->addAction(action);
    d_->toolsToolbar->addAction(action);
    d_->commandManager.registerCommand("SelectFaces", action, cmd);

    action = new QAction("SelectVertices", this);
    cmd = new SelectCommand;
    cmd->getConfig().setInt(SELECT_TYPE, ST_Vertex);
    action->setToolTip("Select vertices.");
    action->setCheckable(true);
    action->setIcon(QIcon(":/img_select.png"));
    d_->menuTools->addAction(action);
    d_->toolActionGroup->addAction(action);
    d_->toolsToolbar->addAction(action);
    d_->commandManager.registerCommand("SelectVertices", action, cmd);

    cmd = new OrbitCommand;
    action = new QAction("Orbit", this);
    action->setCheckable(true);
    action->setToolTip("Orbit view");
    d_->menuTools->addAction(action);
    d_->toolActionGroup->addAction(action);
    d_->toolsToolbar->addAction(action);
    d_->commandManager.registerCommand("Orbit", action, cmd);

	action = new QAction("Move", this);
	cmd = new TransformCommand;
    cmd->getConfig().setInt(CONF_ACTION, TransformCommand::Move);
	Q_CHECK_PTR(action);
	Q_CHECK_PTR(cmd);
	action->setToolTip("Move an object.");
    action->setCheckable(true);
	//action->setEnabled(false);
	d_->menuTools->addAction(action);
	d_->toolActionGroup->addAction(action);
	d_->toolsToolbar->addAction(action);
	d_->commandManager.registerCommand("Move", action, cmd);
    
    action = new QAction("Rotate", this);
	cmd = new TransformCommand;
    cmd->getConfig().setInt(CONF_ACTION, TransformCommand::Rotate);
	Q_CHECK_PTR(action);
	Q_CHECK_PTR(cmd);
	action->setToolTip("Rotate an object.");
    action->setCheckable(true);
	//action->setEnabled(false);
	d_->menuTools->addAction(action);
	d_->toolActionGroup->addAction(action);
	d_->toolsToolbar->addAction(action);
	d_->commandManager.registerCommand("Rotate", action, cmd);

    action = new QAction("Brush", this);
    cmd = new BrushCommand;
    Q_CHECK_PTR(action);
    Q_CHECK_PTR(cmd);
    action->setText("Brush");
    action->setToolTip("Deform the object using different kinds of brushes.");
    action->setCheckable(true);
    action->setIcon(QIcon(":/img_brush.png"));
    d_->menuTools->addAction(action);
    d_->toolActionGroup->addAction(action);
    d_->toolsToolbar->addAction(action);
    d_->commandManager.registerCommand("Brush", action, cmd);

    d_->menuTools->addSeparator();
    d_->toolsToolbar->addSeparator();

    action = new QAction("Subdivide", this);
    cmd = new SubdivideCommand;
    Q_CHECK_PTR(action);
    Q_CHECK_PTR(cmd);
    action->setToolTip("Subdivides each object face.");
    action->setIcon(QIcon(":/img_subdivide.png"));
    d_->menuTools->addAction(action);
    d_->toolsToolbar->addAction(action);
    d_->commandManager.registerCommand("Subdivide", action, cmd);
    
    action = new QAction("EditSubdivide", this);
    cmd = new EditSubdivideCommand;
    Q_CHECK_PTR(action);
    Q_CHECK_PTR(cmd);
    action->setToolTip("Move subdivision control points");
    action->setIcon(QIcon(":/img_subdivide.png"));
    d_->menuTools->addAction(action);
    d_->toolsToolbar->addAction(action);
    d_->commandManager.registerCommand("EditSubdivide", action, cmd);

    cmd = new SmoothSurfaceCommand;
    action = new QAction("SmoothSurface", this);
    d_->menuTools->addAction(action);
    d_->toolsToolbar->addAction(action);
    d_->commandManager.registerCommand("SmoothSurface", action, cmd);
    
    cmd = new TestCommand;
    action = new QAction("Test", this);
    d_->menuTools->addAction(action);
    d_->toolsToolbar->addAction(action);
    d_->commandManager.registerCommand("Test", action, cmd);
    
    connect(&d_->commandManager, SIGNAL(commandActivated(QString)),
            this, SLOT(commandActivated(QString)));

    // Activate default tool
    d_->commandManager.setActiveCommand("Select");

    connect(d_->m_new,SIGNAL(triggered()),this,SLOT(newFile()));
    connect(d_->m_save, SIGNAL(triggered()), this, SLOT(save()));
    connect(d_->m_saveAs, SIGNAL(triggered()), this, SLOT(saveAs()));
    connect(d_->m_open, SIGNAL(triggered()), this, SLOT(open()));

    if (layout())
    {
    	layout()->setContentsMargins(0, 0, 0, 0);
    	layout()->setSpacing(0);
    }

    d_->console->consoleWindow()->setParent(this);
    this->addDockWidget(Qt::BottomDockWidgetArea, d_->console->consoleWindow());
    
    addDockWidget(Qt::RightDockWidgetArea, d_->docTree);
    
    d_->document = std::make_shared<Document>();
    d_->docModel = std::make_shared<DocumentModel>(d_->document);
    d_->documentView->setDocument(d_->document);
}

IDocument::const_shared_ptr QSculptWindow::getCurrentDocument() const
{
    return d_->document;
}

IDocument::shared_ptr QSculptWindow::getCurrentDocument()
{
    return d_->document;
}

DocumentView* QSculptWindow::getCurrentView() const
{
    return d_->documentView;
}

QWidget* QSculptWindow::toolWidget(const QString & key) const
{
    if (key == "DocTree") {
        return d_->docTree;
    }
    return NULL;
}

void QSculptWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        writeSettings();
        d_->console->consoleWindow()->close();
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
        
        d_->document = std::make_shared<Document>();
        d_->docModel = std::make_shared<DocumentModel>(d_->document);
        d_->documentView->setDocument(d_->document);
        
        auto node = std::make_shared<SurfaceNode>(new ::Box, "test");
        auto mat = std::make_shared<PhongMaterial>();
        mat->load();
        node->setMaterial(mat);
        d_->docModel->addItem(node);
        CameraNode::shared_ptr cam = std::make_shared<CameraNode>();
        d_->docModel->addItem(cam);
        d_->docTree->setDocument(d_->docModel);
    }
}

void QSculptWindow::open()
{
    if (maybeSave()) {
        setCurrentFile("");
        
        d_->document = std::make_shared<Document>();
        d_->docModel = std::make_shared<DocumentModel>(d_->document);
        QString fileName = QFileDialog::getOpenFileName(this);
        if (!fileName.isEmpty())
            loadFile(fileName);
        d_->documentView->setDocument(d_->document);        
        d_->docTree->setDocument(d_->docModel);
    }
}

bool QSculptWindow::save()
{
    if (d_->curFile.isEmpty()) {
        return saveAs();
    } else {
        return saveFile(d_->curFile);
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
    QSettings settings("plastlinalabs", "QSculpt");
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(400, 400)).toSize();
    resize(size);
    move(pos);
}

void QSculptWindow::writeSettings()
{
    QSettings settings("plastilinalabs", "QSculpt");
    settings.setValue("pos", pos());
    settings.setValue("size", size());
}

bool QSculptWindow::maybeSave()
{
    return true;
}

void QSculptWindow::loadFile(const QString &fileName)
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    if (d_->document)
    {
        d_->document->loadFile( fileName.toStdString() );
    }
    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File loaded"), 2000);
}

bool QSculptWindow::saveFile(const QString &fileName)
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    if (d_->document)
    {
        d_->document->saveFile( fileName.toStdString() );
    }
    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File saved"), 2000);
    return true;
}

void QSculptWindow::setCurrentFile(const QString &fileName)
{
    d_->curFile = fileName;
    setWindowModified(false);

    QString shownName;
    if (d_->curFile.isEmpty())
        shownName = "untitled.txt";
    else
        shownName = strippedName(d_->curFile);

    setWindowTitle(tr("%1[*] - %2").arg(shownName).arg(tr("Application")));
}

QString QSculptWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

void QSculptWindow::showGrid(bool val)
{
    d_->documentView->setGridVisible( val );
}

ICommand* QSculptWindow::getSelectedCommand() const
{
    return d_->commandManager.getActiveCommand();
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
        QWidget* wid = d_->dockCommandOptions->widget();
        if (wid)
        {
            wid->hide();
        }
        // Set the new widget on the dock and be sure
        // it's visible
        d_->dockCommandOptions->setWidget(widget);
        if (widget)
        {
            widget->show();
            d_->dockCommandOptions->show();
        }
    }
}

void QSculptWindow::commandActivated(QString name)
{
    qDebug() << "QSculptWindow::commandActivated";
    Q_UNUSED(name);
    ICommand* cmd = d_->commandManager.getActiveCommand();
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
        if (d_->dockCommandOptions)
        {
            d_->dockCommandOptions->setWindowOpacity(0.75);
            d_->documentView->grabMouse(true);
        }
    }
    else
    {
        showNormal();
        if (d_->dockCommandOptions)
        {
            d_->dockCommandOptions->setWindowOpacity(1.0);
            d_->documentView->grabMouse(false);
        }
    }
}
