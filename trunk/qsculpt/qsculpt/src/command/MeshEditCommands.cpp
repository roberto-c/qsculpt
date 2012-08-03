/* 
 * File:   MeshEditCommands.cpp
 * Author: rcabral
 * 
 * Created on October 17, 2010, 4:38 PM
 */
#include "Stable.h"
#include "command/MeshEditCommands.h"
#include "ISurface.h"
#include "IDocument.h"
#include "Document.h"
#include "QSculptWindow.h"
#include "QSculptApp.h"
#include "IConfigContainer.h"
#include "subdivision/Box.h"
#include "Scene.h"
#include "SceneNode.h"
#include "GlView.h"
#include "BOManager.h"
#include "DocumentView.h"
#include "HEdge.h"
#include "Octree.h"
#include "Color.h"
#include "DocumentTreeWidget.h"

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
    IDocument::shared_ptr doc = g_pApp->getMainWindow()->getCurrentDocument();
    DocumentTreeWidget * treewdt = qobject_cast<DocumentTreeWidget*>( g_pApp->getMainWindow()->toolWidget("DocTree"));
    if (doc && treewdt)
    {
        QString cmdName = _configContainer->getString("0");
        QString cmdArg1 = _configContainer->getString("1");
        if (cmdArg1.isEmpty()) cmdArg1 = "Unamed";
        _surface = std::make_shared<SurfaceNode>(cmdArg1.toStdString(), new Box);
        _surface->surface()->setColor(Color(0.3f, 0.3f, 0.3f, 1.0f));
        QModelIndexList list = treewdt->selectedIndexes();
        if (list.length() == 1) {
            QModelIndex index = list.first();
            doc->addItem(_surface, index);
        } else {
            doc->addItem(_surface);
        }
        qDebug() << "IID=" << _surface->iid();
    }
}

void AddSurfaceCommand::undo()
{
//    IDocument::shared_ptr doc = g_pApp->getMainWindow()->getCurrentDocument();
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
    
    IDocument::shared_ptr doc = g_pApp->getMainWindow()->getCurrentDocument();
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



struct SmoothSurfaceCommand::Impl {
    ISurface * surf;
    std::map<int, Vector3> oldPosition;
    std::map<int, Vector3> oldNormal;
    
    Impl() : surf(NULL)
    {}
    
    Impl(const Impl& cpy)
    : 
    surf(cpy.surf),
    oldPosition(cpy.oldPosition),
    oldNormal(cpy.oldNormal)
    {
    }
    
    void computeNormals()
    {
        Vector3 normal;
        int counter = 0;
        Iterator<Vertex> it = surf->vertexIterator();
        while(it.hasNext()){
            auto vtx = it.next();
            normal.setZero();
            Iterator<Face> it2 = vtx->faceIterator();
            counter = 0;
            while (it2.hasNext()) {
                counter++;
                normal += computeFaceNormal(it2.next());
            }
            normal = normal / counter;
            vtx->normal() = normal.normalized();
            
            qDebug() << "Vtx num neighbor: " << counter;
        }
    }
    
    Vector3 computeFaceNormal(Face::shared_ptr f)
    {
        Iterator<Edge> it = f->edgeIterator();
        auto e1 = it.next();
        auto e2 = it.next();
        Vector3 v1 = e2->tail()->position() - e2->head()->position();
        Vector3 v2 = e1->head()->position() - e1->tail()->position();
        return v1.cross(v2).normalized();
    }
};


SmoothSurfaceCommand::SmoothSurfaceCommand()
:
CommandBase("SmoothSurface"),
d_(new Impl)
{
    
}

SmoothSurfaceCommand::SmoothSurfaceCommand(const SmoothSurfaceCommand& orig)
: 
CommandBase(orig),
d_(new Impl(*orig.d_))
{
    
}
SmoothSurfaceCommand::~SmoothSurfaceCommand()
{
    
}


ICommand* SmoothSurfaceCommand::clone() const
{
    return new SmoothSurfaceCommand(*this);
}

void SmoothSurfaceCommand::execute()
{
    qDebug() << "Smooth Surface" ;
    IDocument::shared_ptr doc = g_pApp->getMainWindow()->getCurrentDocument();
    if (doc)
    {
        auto node = doc->getSelectedObjects().at(0).lock();
        auto surfNode = std::dynamic_pointer_cast<SurfaceNode>(node);
        d_->surf = surfNode ? surfNode->surface() : NULL;
    }
    if (d_->surf == NULL) {
        // no surface to operate
        qDebug() << "No surface to operate" ;
        return;
    }
    
    // Build octree
//    data::Octree<Vertex*> vtxIdx;
//    Iterator<Vertex> vtxIt = d_->surf->vertexIterator();
//    while (vtxIt.hasNext()) {
//        auto v = vtxIt.next();
//        vtxIdx.add(v);
//    }
//    geometry::AABB box;
//    vtxIdx.findIntersect(box, NULL);
    d_->computeNormals();
    d_->surf->setChanged(true);
    g_pApp->getMainWindow()->getCurrentView()->updateView();
    
    qDebug() << "Smooth Surface end" ;
}
void SmoothSurfaceCommand::undo()
{
    
}
void SmoothSurfaceCommand::redo()
{
    
}


AddPointCommand::AddPointCommand()
{
    
}
AddPointCommand::AddPointCommand(const AddPointCommand& orig)
{
    
}
AddPointCommand::~AddPointCommand()
{
    
}
    
ICommand* AddPointCommand::clone() const
{
    return new AddPointCommand(*this);
}

void AddPointCommand::execute()
{
    
}

void AddPointCommand::undo()
{
    
}

void AddPointCommand::redo()
{
    
}


AddFaceCommand::AddFaceCommand()
{
    
}

AddFaceCommand::AddFaceCommand(const AddFaceCommand& orig)
{
    
}

AddFaceCommand::~AddFaceCommand()
{
    
}

ICommand* AddFaceCommand::clone() const
{
    return new AddFaceCommand(*this);
}

void AddFaceCommand::execute()
{
    
}

void AddFaceCommand::undo()
{
    
}

void AddFaceCommand::redo()
{
    
}

struct TestCommand::Impl {
    Document            doc;
    SceneNode::shared_ptr root;
    ISurface            *surf;
    Vertex              *vtx;
    Iterator<Face>      faceIt;
    Iterator<Vertex>    vtxIt;
    
    Impl():surf(0),vtx(0){}
    Impl(const Impl& cpy):surf(0),vtx(0){}
    
    void setup();
};

void TestCommand::Impl::setup() {
    root = SceneNode::shared_ptr(new SceneNode);
    surf = new Subdivision;
    auto sceneptr = doc.scene().lock();
    std::vector<int> vertexID;
    
    if (!root || !surf || !sceneptr) {
        throw std::bad_alloc();
    }
    
    //
    //   6  5  4
    //   7  8  3
    //   0  1  2
    //
    vertexID.push_back(surf->addVertex(Point3(0.0f, 0.0f, 0))); // 0
    vertexID.push_back(surf->addVertex(Point3(0.5f, 0.0f, 0))); // 1
    vertexID.push_back(surf->addVertex(Point3(1.0f, 0.0f, 0))); // 2
    vertexID.push_back(surf->addVertex(Point3(1.0f, 0.5f, 0))); // 3
    vertexID.push_back(surf->addVertex(Point3(1.0f, 1.0f, 0))); // 4
    vertexID.push_back(surf->addVertex(Point3(0.5f, 1.0f, 0))); // 5
    vertexID.push_back(surf->addVertex(Point3(0.0f, 1.0f, 0))); // 6
    vertexID.push_back(surf->addVertex(Point3(0.0f, 0.5f, 0))); // 7
    vertexID.push_back(surf->addVertex(Point3(0.5f, 0.5f, 0))); // 8
    
    std::vector<size_t> face;
    face.clear();
    face.push_back(vertexID[0]);
    face.push_back(vertexID[1]);
    face.push_back(vertexID[8]);
    face.push_back(vertexID[7]);
    qDebug() << "new face IID:" << surf->addFace(face);

    face.clear();
    face.push_back(vertexID[1]);
    face.push_back(vertexID[2]);
    face.push_back(vertexID[3]);
    face.push_back(vertexID[8]);
    qDebug() << "new face IID:" << surf->addFace(face);

    face.clear();
    face.push_back(vertexID[8]);
    face.push_back(vertexID[3]);
    face.push_back(vertexID[4]);
    face.push_back(vertexID[5]);
    qDebug() << "new face IID:" << surf->addFace(face);

    face.clear();
    face.push_back(vertexID[7]);
    face.push_back(vertexID[8]);
    face.push_back(vertexID[5]);
    face.push_back(vertexID[6]);
    qDebug() << "new face IID:" << surf->addFace(face);
    
    SurfaceNode::shared_ptr ptr(new SurfaceNode(surf));
    ptr->setParent(root);
    root->add(ptr);
    
    vtxIt = surf->vertexIterator();
    vtx = surf->vertex(vertexID[3]);
    if (vtx) {
        faceIt = vtx->faceIterator();
        vtxIt = vtx->vertexIterator();
    }

    
    root->transform() *= Eigen::Translation<float,3>(0.5f,0.5f,0);
    sceneptr->add(root);
}

TestCommand::TestCommand()
: CommandBase("Test"),
 _d(new Impl)
{
    _d->setup();
}

TestCommand::TestCommand(const TestCommand& orig)
: CommandBase(orig),
_d(new Impl(*orig._d))
{
    _d->setup();
}

TestCommand::~TestCommand()
{
    
}
    

ICommand* TestCommand::clone() const
{
    return new TestCommand(*this);
}

bool TestCommand::needsUserInteraction() const
{
    return true;
}

void TestCommand::execute()
{
    
}

void TestCommand::undo()
{
    
}

void TestCommand::redo()
{
    
}

QWidget* TestCommand::getOptionsWidget()
{
    return nullptr;
}

/**
 *
 */
void TestCommand::mousePressEvent(QMouseEvent *e)
{
    qDebug() << "Test: press";
//    if (_d->faceIt.hasNext()) {
//        _d->faceIt.peekNext().addFlag(FF_Selected);
//        qDebug() << _d->faceIt.peekNext().iid();
//        _d->faceIt.next();
//        _d->surf->setChanged(true);
//    } else {
//        qDebug() << "No more elements";
//    }
    
    if (_d->vtxIt.hasNext()) {
        auto v = _d->vtxIt.next();
        v->color() = Vector3(1,0.3f,0);
        qDebug() << v->iid();
        
        _d->surf->setChanged(true);
    } else {
        qDebug() << "No more elements";
    }
}

/**
 * Called when a mouse release event ocurrs. This method is called by the
 * widget (a QGLWidget).
 *
 */
void TestCommand::mouseReleaseEvent(QMouseEvent *e)
{
    qDebug() << "Test: release";
}

/**
 * Called when a mouse move event ocurrs. This method is called by the
 * widget (a QGLWidget).
 *
 */
void TestCommand::mouseMoveEvent(QMouseEvent *e)
{
}

/**
 * Used to display anything specific to the command as user feedback.
 */
void TestCommand::paintGL(GlCanvas *c)
{
    auto s = _d->doc.scene().lock();
    c->drawScene(s);
}

