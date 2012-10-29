/* 
 * File:   MeshEditCommands.h
 * Author: rcabral
 *
 * Created on October 17, 2010, 4:38 PM
 */

#ifndef MESHEDITCOMMANDS_H
#define	MESHEDITCOMMANDS_H

#include "command/CommandBase.h"

#include <PlastilinaCore/SceneNode.h>

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
    SurfaceNode::shared_ptr _surface;
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
    SurfaceNode::shared_ptr _surface;
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


class SmoothSurfaceCommand : public CommandBase
{
public:
    SmoothSurfaceCommand();
    SmoothSurfaceCommand(const SmoothSurfaceCommand& orig);
    virtual ~SmoothSurfaceCommand();
    
    // ICommand Interface
    virtual ICommand* clone() const;
    virtual bool needsUserInteraction() const { return false; }
    virtual void execute();
    virtual void undo();
    virtual void redo();
    virtual QWidget* getOptionsWidget(){return NULL;}
    // End ICommand Interface
    
private:
    struct Impl;
    QScopedPointer<Impl> d_;
};

class AddPointCommand : public CommandBase
{
    AddPointCommand();
    AddPointCommand(const AddPointCommand& orig);
    virtual ~AddPointCommand();
    
    // ICommand Interface
    virtual ICommand* clone() const;
    virtual bool needsUserInteraction() const { return false; }
    virtual void execute();
    virtual void undo();
    virtual void redo();
    virtual QWidget* getOptionsWidget(){return NULL;}
    // End ICommand Interface
};

class AddFaceCommand : public CommandBase
{
    AddFaceCommand();
    AddFaceCommand(const AddFaceCommand& orig);
    virtual ~AddFaceCommand();
    
    // ICommand Interface
    virtual ICommand* clone() const;
    virtual bool needsUserInteraction() const { return false; }
    virtual void execute();
    virtual void undo();
    virtual void redo();
    virtual QWidget* getOptionsWidget(){return NULL;}
    // End ICommand Interface
};


class TestCommand : public CommandBase {
public:
    TestCommand();
    TestCommand(const TestCommand& orig);
    virtual ~TestCommand();
    
    // ICommand Interface
    virtual ICommand* clone() const;
    virtual bool needsUserInteraction() const;
    virtual void execute();
    virtual void undo();
    virtual void redo();
    virtual QWidget* getOptionsWidget();
    /**
     *
     */
    virtual void mousePressEvent(QMouseEvent *e);
    
    /**
     * Called when a mouse release event ocurrs. This method is called by the
     * widget (a QGLWidget).
     *
     */
    virtual void mouseReleaseEvent(QMouseEvent *e);
    
    /**
     * Called when a mouse move event ocurrs. This method is called by the
     * widget (a QGLWidget).
     *
     */
    virtual void mouseMoveEvent(QMouseEvent *e);
    
    /**
     * Used to display anything specific to the command as user feedback.
     */
    virtual void paintGL(GlCanvas *c);
    // End ICommand Interface
    
private:
    struct Impl;
    QScopedPointer<Impl> _d;
};

#endif	/* MESHEDITCOMMANDS_H */

