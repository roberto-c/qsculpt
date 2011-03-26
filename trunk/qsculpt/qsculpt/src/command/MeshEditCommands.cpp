/* 
 * File:   MeshEditCommands.cpp
 * Author: rcabral
 * 
 * Created on October 17, 2010, 4:38 PM
 */
#include "Stable.h"
#include "MeshEditCommands.h"
#include "ISurface.h"
#include "IDocument.h"
#include "QSculptWindow.h"
#include "QSculptApp.h"
#include "IConfigContainer.h"
#include "Box.h"
#include "Scene.h"
#include "SceneNode.h"

AddSurfaceCommand::AddSurfaceCommand()
: CommandBase("AddSurface")
    , _surface(NULL){
}

AddSurfaceCommand::AddSurfaceCommand(const AddSurfaceCommand& orig)
: CommandBase(orig),
  _surface(orig._surface)
{
}

AddSurfaceCommand::~AddSurfaceCommand()
{
}

ICommand* AddSurfaceCommand::clone() const
{
    return new AddSurfaceCommand(*this);
}

void AddSurfaceCommand::execute()
{
    IDocument* doc = g_pApp->getMainWindow()->getCurrentDocument();
    if (doc)
    {
        _surface = new SurfaceNode(new Box);
        doc->scene()->appendRow(_surface);
        qDebug() << "IID=" << _surface->iid();
    }
}

void AddSurfaceCommand::undo()
{
//    IDocument* doc = g_pApp->getMainWindow()->getCurrentDocument();
//    if (doc && _surface)
//    {
//        doc->removeObject(_surface);
//        delete _surface;
//        _surface = NULL;
//    }
}

void AddSurfaceCommand::redo()
{
    execute();
}



RemoveSurfaceCommand::RemoveSurfaceCommand()
: CommandBase("RemoveSurface")
, _surface(NULL){
}

RemoveSurfaceCommand::RemoveSurfaceCommand(const RemoveSurfaceCommand& orig)
: CommandBase(orig),
_surface(orig._surface)
{
}

RemoveSurfaceCommand::~RemoveSurfaceCommand()
{
}

ICommand* RemoveSurfaceCommand::clone() const
{
    return new RemoveSurfaceCommand(*this);
}

void RemoveSurfaceCommand::execute()
{
    int iid = _configContainer->getInt("IID");
    if (iid == 0 ) return;
    
    IDocument* doc = g_pApp->getMainWindow()->getCurrentDocument();
    if (!doc) return;
    
    NOT_IMPLEMENTED
}

void RemoveSurfaceCommand::undo()
{
    NOT_IMPLEMENTED
}

void RemoveSurfaceCommand::redo()
{
    execute();
}


AddGroupCommand::AddGroupCommand()
: CommandBase("AddGroup")
{
}

AddGroupCommand::AddGroupCommand(const AddGroupCommand& orig)
: CommandBase(orig)
{
}

AddGroupCommand::~AddGroupCommand()
{
}


ICommand* AddGroupCommand::clone() const
{
    return new AddGroupCommand(*this);
}

void AddGroupCommand::execute()
{
    NOT_IMPLEMENTED
}

void AddGroupCommand::undo()
{
    NOT_IMPLEMENTED
}

void AddGroupCommand::redo()
{
    NOT_IMPLEMENTED
}

