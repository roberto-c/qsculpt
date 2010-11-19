/* 
 * File:   MeshEditCommands.h
 * Author: rcabral
 *
 * Created on October 17, 2010, 4:38 PM
 */

#ifndef MESHEDITCOMMANDS_H
#define	MESHEDITCOMMANDS_H

#include "CommandBase.h"

class ISurface;

class AddSurfaceCommand : public CommandBase {
public:
    AddSurfaceCommand();
    AddSurfaceCommand(const AddSurfaceCommand& orig);
    virtual ~AddSurfaceCommand();

        // ICommand Interface
    virtual ICommand* clone() const;
    virtual bool needsUserInteraction() const { return false; }
    virtual void execute();
    virtual void undo();
    virtual void redo();
    virtual QWidget* getOptionsWidget(){return NULL;}
    // End ICommand Interface

private:
    ISurface* _surface;
};

class RemoveSurfaceCommand : public CommandBase 
{
public:
    
    RemoveSurfaceCommand();
    RemoveSurfaceCommand(const RemoveSurfaceCommand& orig);
    virtual ~RemoveSurfaceCommand();
    
    // ICommand Interface
    virtual ICommand* clone() const;
    virtual bool needsUserInteraction() const { return false; }
    virtual void execute();
    virtual void undo();
    virtual void redo();
    virtual QWidget* getOptionsWidget(){return NULL;}
    // End ICommand Interface
    
private:
    ISurface* _surface;
};

#endif	/* MESHEDITCOMMANDS_H */

