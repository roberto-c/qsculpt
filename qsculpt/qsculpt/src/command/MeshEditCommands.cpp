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
        _surface = doc->addObject(IDocument::Box);
    }
}

void AddSurfaceCommand::undo()
{
    IDocument* doc = g_pApp->getMainWindow()->getCurrentDocument();
    if (doc && _surface)
    {
        doc->removeObject(_surface);
        delete _surface;
        _surface = NULL;
    }
}

void AddSurfaceCommand::redo()
{
    execute();
}

