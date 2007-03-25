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
#include <QtGui>
#include <QtOpenGL>
#include <QDockWidget>
#include <QFile>
#include <QTextStream>

#include "qsculptwindow.h"
#include "documentview.h"
#include "document.h"
#include "movecommand.h"
#include "transformwidget.h"
#include "selectcommand.h"
#include "brushcommand.h"
#include "subdividecommand.h"

QSculptWindow::QSculptWindow()
    : m_glWidget(new DocumentView(this)),
    m_document(new Document),
    m_currentCommand(NULL),
    m_toolActionGroup(NULL),
    m_dockCommandOptions(NULL),
    m_toolsToolbar(NULL)
{
    createWidgets();
    
    readSettings();
}

QSculptWindow::~QSculptWindow()
{
    delete m_document;
    //delete m_moveCmd;
}

void QSculptWindow::createWidgets()
{
    setupUi(this);
    m_glWidget->setDocument(m_document);
    
    setCentralWidget(m_glWidget);
    
    m_dockCommandOptions = new QDockWidget("Options", this);
    m_dockCommandOptions->setAllowedAreas(Qt::NoDockWidgetArea /*Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea*/);
    //addDockWidget(Qt::NoDockWidgetArea, m_dockCommandOptions);
	m_dockCommandOptions->setFloating(true);
	m_dockCommandOptions->move(20, 50);

	//m_dockCommandOptions->setWindowOpacity(0.85);
	
    connect(m_showGrid, SIGNAL(toggled(bool)), m_glWidget, SLOT(setGridVisible(bool)));
    connect(m_showNormals, SIGNAL(toggled(bool)), m_glWidget, SLOT(setNormalsVisible(bool)));
    connect(m_document, SIGNAL(changed(IDocument::ChangeType, IObject3D*)), this, SLOT(documentChanged(IDocument::ChangeType)));
    
    connect(m_addBox, SIGNAL(activated()), this, SLOT(addBox()));
    connect(m_addSphere, SIGNAL(activated()), this, SLOT(addSphere()));
    
    QVariant v;
    ICommand* cmd;
    v.setValue((QObject*)(cmd = new SelectCommand));
    m_select->setData( v );
    m_select->setCheckable(true);
    connect(m_select, SIGNAL(activated()), this, SLOT(activateCommand()));
    
    v.setValue((QObject*)(cmd = new TransformCommand));
    m_transform->setData( v );
    m_transform->setCheckable(true);
    connect(m_transform, SIGNAL(activated()), this, SLOT(activateCommand()));
    
    v.setValue((QObject*)(cmd = new BrushCommand));
    m_brush->setData( v );
    m_brush->setCheckable(true);
    connect(m_brush, SIGNAL(activated()), this, SLOT(activateCommand()));
    
    v.setValue((QObject*)(cmd = new SubdivideCommand));
    m_subdivide->setData( v );
    connect(m_subdivide, SIGNAL(activated()), this, SLOT(executeCommand()));
    
    m_toolActionGroup = new QActionGroup(this);
    m_toolActionGroup->addAction(m_select);
    m_toolActionGroup->addAction(m_transform);
    m_toolActionGroup->addAction(m_brush);
    
    // Create toolbar
    m_toolsToolbar = addToolBar("Tools");
    m_toolsToolbar->addAction(m_select);
    m_toolsToolbar->addAction(m_transform);
    m_toolsToolbar->addAction(m_brush);
    m_toolsToolbar->addSeparator();
    m_toolsToolbar->addAction(m_subdivide);
    
    // Activate default tool
    m_select->activate(QAction::Trigger);
    
    connect(m_save, SIGNAL(activated()), this, SLOT(save()));
    connect(m_saveAs, SIGNAL(activated()), this, SLOT(saveAs()));
    connect(m_open, SIGNAL(activated()), this, SLOT(open()));
}

const IDocument* QSculptWindow::getCurrentDocument()
{
    return m_document;
}

DocumentView* QSculptWindow::getCurrentView() const
{
    return m_glWidget;
}

void QSculptWindow::closeEvent(QCloseEvent *event)
{
     if (maybeSave()) {
         writeSettings();
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
    if (curFile.isEmpty()) {
        return saveAs();
    } else {
        return saveFile(curFile);
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
    curFile = fileName;
    //textEdit->document()->setModified(false);
    setWindowModified(false);

    QString shownName;
    if (curFile.isEmpty())
        shownName = "untitled.txt";
    else
        shownName = strippedName(curFile);

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
}

void QSculptWindow::showGrid(bool val)
{
    m_glWidget->setGridVisible( val );
}

ICommand* QSculptWindow::getSelectedCommand() const
{
    return m_currentCommand;
}

void QSculptWindow::documentChanged(IDocument::ChangeType /*type*/)
{
//     TransformWidget* widget = (TransformWidget*)m_dockTransform->widget();
//     if (m_document->getObjectsCount() > 0)
//         widget->enable( true );
//     else
//         widget->enable( false );
}

void QSculptWindow::activateCommand()
{
    QObject* obj = sender();
    if (obj && obj->inherits("QAction"))
    {
        ICommand* cmd = NULL;
        QVariant value = ((QAction*)obj)->data();
        if (value.isValid())
        {
            cmd = static_cast<ICommand*>( value.value<QObject*>() );
            if (cmd)
            {
				if (m_currentCommand)
					m_currentCommand->activate(false);
				
                m_currentCommand = (ICommand*) cmd;
				
				m_currentCommand->activate(true);
            }
        }
    }
}

void QSculptWindow::executeCommand()
{
    qDebug("executeCommand()");
    QObject* obj = sender();
    if (obj && obj->inherits("QAction"))
    {
        ICommand* cmd = NULL;
        QVariant value = ((QAction*)obj)->data();
        if (value.isValid())
        {
            cmd = dynamic_cast<ICommand*>(value.value<QObject*>());
            if (cmd)
            {
                cmd->execute();
            }
            else
            {
                qDebug("command cast failed");
            }
        }
    }
}

void QSculptWindow::setOptionsWidget(QWidget* widget)
{
	m_dockCommandOptions->setWidget(widget);
	m_dockCommandOptions->show();
}
