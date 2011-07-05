/* 
 * File:   MeshEditCommands.h
 * Author: rcabral
 *
 * Created on October 17, 2010, 4:38 PM
 */

#ifndef MESHEDITCOMMANDS_H
#define	MESHEDITCOMMANDS_H

#include "command/CommandBase.h"

class SurfaceNode;

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
    SurfaceNode* _surface;
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
    SurfaceNode* _surface;
};


class AddGroupCommand : public CommandBase
{
public:
    AddGroupCommand();
    AddGroupCommand(const AddGroupCommand& orig);
    ~AddGroupCommand();
    
    // ICommand Interface
    virtual ICommand* clone() const;
    virtual bool needsUserInteraction() const { return false; }
    virtual void execute();
    virtual void undo();
    virtual void redo();
    virtual QWidget* getOptionsWidget(){return NULL;}
    // End ICommand Interface
    
};

#endif	/* MESHEDITCOMMANDS_H */

