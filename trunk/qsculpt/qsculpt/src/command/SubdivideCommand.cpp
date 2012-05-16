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
#include "command/SubdivideCommand.h"
#include <QtCore/QtDebug>
#include <QtCore/QThread>
#include <QtGui/QProgressDialog>
//#include <omp.h>
#include "IDocument.h"
#include "ISurface.h"
#include "QSculptApp.h"
#include "QSculptWindow.h"
#include "DocumentView.h"
#include "HEdge.h"
#include "Face.h"
#include "subdivision/Subdivision.h"
#include "PointRenderer.h"

typedef QHash< std::pair<int, int>, Vertex*> MidEdgeMap;

// Vertex flags:
// VF_User1 => True if the vertex is an original point (was before the 
//             subdivision iteration)

struct SubdivideCommand::Impl {
    MidEdgeMap edgeMidPoint;
    
    Impl(){}
    Impl(const Impl& cpy){}
    
    void splitEdges(ISurface& s);
    
    void addFaceMidPointVertex(ISurface& s);
    
    void createFaces(ISurface& s);
    
    void removeOldFaces(ISurface& s);
    
    void computeNewPosition(ISurface& s);
    
//    void computeBorderPosition(ISurface& s,
    
    void cleanUserData(ISurface& s);
    
    void smoothNormals(ISurface& s);
    
    Vector3 computeFaceNormal(Face::WeakPtr f);
    
    void diagnostiscs(ISurface & s);
};

SubdivideCommand::SubdivideCommand()
    : CommandBase("Subdivide")
{
}

SubdivideCommand::SubdivideCommand(const SubdivideCommand& cpy)
    : CommandBase(cpy)
, _d(new Impl(*cpy._d.data()))
{
}

SubdivideCommand::~SubdivideCommand()
{
}

ICommand* SubdivideCommand::clone() const
{
    return new SubdivideCommand(*this);
}

void SubdivideCommand::undo()
{
}

void SubdivideCommand::redo()
{
}

void SubdivideCommand::execute()
{
    //if (activate == false)
    //	return;

    qDebug("execute SubdivideCommand()");
    QProgressDialog dlg("Subdividing the selected object...", 0, 0, 100,
                        g_pApp->getMainWindow());
    dlg.setWindowModality(Qt::WindowModal);
    dlg.setAutoReset(true);
    dlg.setAutoClose(true);
    dlg.setValue(0);

    //    QThread::connect(m_workerThread, SIGNAL(progress(int)),
    //					 &dlg, SLOT(setValue(int)));

    dlg.show();

    auto doc = g_pApp->getMainWindow()->getCurrentDocument();

    if (!doc)
        return;

    _d->edgeMidPoint.clear();
    qDebug() << "Start time: " <<QDateTime::currentDateTime();

    QList<SceneNode::WeakPtr> list = doc->getSelectedObjects();
    foreach(SceneNode::WeakPtr n, list)
    {
        auto node = n.lock();
        if (node->nodeType() != NT_Surface)
            continue;
        ISurface * obj = std::dynamic_pointer_cast<SurfaceNode>(node)->surface();
        qDebug() << "Object found";
        qDebug() << "Num faces =" << obj->numFaces();
        QVector<Face*> facesToDelete;
        
        // mark all orginal points with -1
        Iterator<Vertex> it = obj->vertexIterator();
        while (it.hasNext()) {
            it.next()->addFlag(VF_User1);
        }
        
        //#pragma omp parallel
        {
            // split edges in half and add a vertex per face
            _d->addFaceMidPointVertex(*obj);
            _d->splitEdges(*obj);
            
            _d->computeNewPosition(*obj);
            
            // create new faces
            _d->createFaces(*obj);

            // removeOldFaces
            _d->removeOldFaces(*obj);
            
            // set to null data
            _d->cleanUserData(*obj);
            
            _d->smoothNormals(*obj);
        }
        //_d->diagnostiscs(*obj);
        
        obj->setChanged(true);
        qDebug() << "Num Faces: " << obj->numFaces();
        qDebug() << "Num Vertices: " << obj->numVertices();
    }
    qDebug() << "End time:" << QDateTime::currentDateTime();
    dlg.setValue(100);
    g_pApp->getMainWindow()->getCurrentView()->updateView();
}

void SubdivideCommand::Impl::addFaceMidPointVertex(ISurface& s)
{
    Vector3 p;
    Iterator<Face> faceIt = s.faceIterator();
    // Create new vertices at the mid point of each edge
    while(faceIt.hasNext()){
        Face::SharedPtr f = faceIt.next();
        Iterator<Vertex> vtxIt = f->vertexIterator();
        p.setZero();
        int val = 0;
        while (vtxIt.hasNext()) {
            p += vtxIt.next()->position();
            ++val;
        }
        p = p / val;
        Vertex * vtx = s.vertex(s.addVertex(p));
        f->setUserData( static_cast<void*>(vtx) );
    }
}

void SubdivideCommand::Impl::splitEdges(ISurface& s)
{
    Iterator<Edge> edgeIt = s.edgeIterator();
    // Create new vertices at the mid point of each edge
    while(edgeIt.hasNext()){
        auto e = edgeIt.next();
        if (e->userData() == NULL) {
            // skip edges with no associated faces
            // every edge has to have two faces associated.
            if (e->face() == NULL || e->pair()->face() == NULL) {
                continue;
            }
                
            assert(e->face() && e->face()->userData());
            assert(e->pair() && e->pair()->face() && e->pair()->face()->userData());
            
            Vector3 p = (e->tail()->position() + e->head()->position()) * 0.5f;
            
            Vector3 pos;
            if (e->isFlagSet(EF_Crease)) {
                pos = p;
            } else {
                Vertex * v1 = static_cast<Vertex*>(e->face()->userData());
                Vertex * v2 = static_cast<Vertex*>(e->pair()->face()->userData());
                Vector3 pf = (v1->position() + v2->position()) * 0.5f;
                pos = (p+pf)/2;
            }
            
            Vertex * vtx = s.vertex(s.addVertex(pos));
            e->setUserData( static_cast<void*>(vtx) );
            assert(e->pair() != NULL && e->pair()->userData() == NULL);
            e->pair()->setUserData( static_cast<void*>(vtx) );
            
        }
    }
}

void SubdivideCommand::Impl::createFaces(ISurface& s)
{
    int faceCounter = 0;
    QVector<size_t> vtxIndex, faceVtxIID;
    vtxIndex.reserve(10);
    faceVtxIID.reserve(4);
    Iterator<Face> faceIt = s.faceIterator();
    // Create new vertices at the mid point of each edge
    while(faceIt.hasNext()){
        auto f = faceIt.next();
        // skip face if it does not contain midpoint as it is probably a new
        // face.
        if (f->userData() == NULL) {
            continue;
        }
        
        vtxIndex.clear();
        size_t numEdges = 0;
        faceCounter++;
        Iterator<Edge> edgeIt = f->edgeIterator();
        while (edgeIt.hasNext()) {
            // for each edge add the tail vertex and the the edge midpoint.
            auto e = edgeIt.next();
            vtxIndex.push_back(e->tail()->iid());
            Vertex * v = static_cast<Vertex*>(e->userData());
            assert(v != NULL);
            vtxIndex.push_back(v->iid());
            if (e->isFlagSet(EF_Crease)) {
                v->addFlag(VF_Crease);
            }
            numEdges++;
        }
        Vertex * v = static_cast<Vertex*>(f->userData());
        assert(v != NULL);
        int faceCenter = v->iid();
        
        size_t numVerts = vtxIndex.size();
        for (size_t i = 0; i < numEdges; ++i) {
            faceVtxIID.clear();
            faceVtxIID.push_back(faceCenter);
            faceVtxIID.push_back(vtxIndex[((i + numEdges)*2 - 1) % numVerts]);
            faceVtxIID.push_back(vtxIndex[i*2]);
            faceVtxIID.push_back(vtxIndex[((i*2)+1) % numVerts]);
            s.addFace(faceVtxIID);
        }
    }
}

void SubdivideCommand::Impl::computeNewPosition(ISurface& s)
{
    Iterator<Vertex> vtxIt = s.vertexIterator();
    // Create new vertices at the mid point of each edge
    while(vtxIt.hasNext()){
        auto v = vtxIt.next();
        bool isOriginalPoint = v->isFlagSet(VF_User1);
        if (isOriginalPoint) {
            int n = 0;
            Vector3 fAvg;
            Iterator<Face> faceIt = v->faceIterator();
            while (faceIt.hasNext()) {
                auto f = faceIt.next();
                if (f->userData() != NULL) {
                    fAvg += static_cast<Vertex*>(f->userData())->position();
                    n++;
                }
            }
            if (n == 0) {
                continue;
            }
            fAvg = fAvg / n;
            
            Vector3 eAvg;
            Iterator<Edge> edgeIt = v->edgeIterator();
            while (edgeIt.hasNext()) {
                auto e = edgeIt.next();
                if (e->userData() != NULL) {
                    eAvg += static_cast<Vertex*>(e->userData())->position();
                }
            }
            eAvg = eAvg / n;
            
            assert(n >= 3);
            v->position() = (fAvg + (eAvg * 2.0f) + v->position()*(n-3)) / n;
            v->removeFlag(VF_User1);
        }
    }
}

void SubdivideCommand::Impl::removeOldFaces(ISurface& s)
{
    std::vector<int> faceIndex;
    Iterator<Face> faceIt = s.faceIterator();
    while(faceIt.hasNext()){
        auto f = faceIt.next();
        // skip face if it does not contain midpoint as it is probably a new
        // face and it should not be removed.
        if (f->userData() == NULL) {
            continue;
        }
        s.removeFace(f->iid());
    }
}

void SubdivideCommand::Impl::cleanUserData(ISurface& s)
{
    Iterator<Edge> edgeIt = s.edgeIterator();
    while(edgeIt.hasNext()){
        auto e = edgeIt.next();
        e->setUserData(NULL);
        e->pair()->setUserData(NULL);
    }
    Iterator<Face> faceIt = s.faceIterator();
    while(faceIt.hasNext()){
        faceIt.next()->setUserData(NULL);
    }
}

void SubdivideCommand::Impl::smoothNormals(ISurface& s)
{
    Vector3 normal;
    int counter = 0;
    Iterator<Vertex> it = s.vertexIterator();
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
        
        //qDebug() << "Vtx num neighbor: " << counter;
    }
}

Vector3 SubdivideCommand::Impl::computeFaceNormal(Face::WeakPtr face)
{
    auto f = face;
    Iterator<Edge> it = f->edgeIterator();
    auto e1 = it.next();
    auto e2 = it.next();
    Vector3 v1 = e2->tail()->position() - e2->head()->position();
    Vector3 v2 = e1->head()->position() - e1->tail()->position();
    return v1.cross(v2).normalized();
}

void SubdivideCommand::Impl::diagnostiscs(ISurface & s)
{
    // print edges without faces
    qDebug() << "Checking edges with no faces associated...";
    Iterator<Edge> edgeIt = s.edgeIterator();
    while (edgeIt.hasNext()) {
        auto e = edgeIt.next();
        if (e->face() == NULL) {
            qDebug() << "Edge with no face: " << e->iid();
        }
    }
    qDebug() << "Checking vertices with no edges associated...";
    Iterator<Vertex> vtxIt = s.vertexIterator();
    while (vtxIt.hasNext()) {
        auto v = vtxIt.next();
        if (v->edge() == NULL) {
            qDebug() << "Vertex with no edge: " << v->iid();
        }
    }
}


struct EditSubdivideCommand::Impl {
    SurfaceNode::SharedPtr  surf;
    PointRenderer   renderer;
    IID             vtxIID;
    Point3          mouseScreen;
    
    Impl() : surf(NULL), vtxIID(0) 
    {
        renderer.setPointSize(10.0f);
    }
    
    Impl(const Impl & cpy) : surf(cpy.surf),vtxIID(cpy.vtxIID) 
    {
        renderer.setPointSize(10.0f);
    }
};

EditSubdivideCommand::EditSubdivideCommand() : d_(new Impl)
{
    
}
    
EditSubdivideCommand::EditSubdivideCommand(const EditSubdivideCommand& cpy)
: d_(new Impl(*cpy.d_.data())){
    
}
    
EditSubdivideCommand::~EditSubdivideCommand(){
    
}
    

ICommand* EditSubdivideCommand::clone() const{
    return new EditSubdivideCommand(*this);
}
    
void EditSubdivideCommand::execute(){
    
}
void EditSubdivideCommand::undo(){
    
}
void EditSubdivideCommand::redo(){
    
}

void EditSubdivideCommand::mousePressEvent(QMouseEvent *e){
    auto doc = g_pApp->getMainWindow()->getCurrentDocument();
    
    if (!doc)
        return;
    QList<SceneNode::WeakPtr> list = doc->getSelectedObjects();
    foreach(SceneNode::WeakPtr n, list)
    {
        auto node = n.lock();
        if (node->nodeType() != NT_Surface){
            continue;
        }
        ISurface * s = std::dynamic_pointer_cast<SurfaceNode>(node)->surface();
        if (dynamic_cast<Subdivision*>(s)) {
            d_->surf = std::dynamic_pointer_cast<SurfaceNode>(node);
            break;
        }
    }
    
    if (d_->surf) {
        GlCanvas * canvas = g_pApp->getMainWindow()->getCurrentView()->getCanvas();
        
        // Get the mouse coordinates to world coordinates.
        float depth = canvas->depth(e->x(), e->y());
        d_->mouseScreen = Point3(e->x(), canvas->height() - e->y(), depth);
        Point3 world = canvas->screenToWorld(d_->mouseScreen);
        // Then, from world coordinate to local objet coordinates
        Point3 p = d_->surf->worldToLocal(world);
        // Now, look for the vertex closest to such position and select it.
        d_->vtxIID = d_->surf->surface()->getClosestPointAtPoint(p);
        d_->surf->surface()->vertex(d_->vtxIID)->addFlag(VF_Selected);
        d_->surf->surface()->setChanged(true);
        
        g_pApp->getMainWindow()->getCurrentView()->updateView();
    }
}
    
void EditSubdivideCommand::mouseReleaseEvent(QMouseEvent *e)
{
    if (d_->surf && d_->vtxIID > 0) {
        d_->surf->surface()->vertex(d_->vtxIID)->removeFlag(VF_Selected);
        d_->surf->surface()->setChanged(true);
        
        g_pApp->getMainWindow()->getCurrentView()->updateView();
    }
}

void EditSubdivideCommand::mouseMoveEvent(QMouseEvent *e)
{
    if (d_->surf && d_->vtxIID > 0) {
        GlCanvas * canvas = g_pApp->getMainWindow()->getCurrentView()->getCanvas();
        float depth = d_->mouseScreen.z();
        d_->mouseScreen = Point3(e->x(), canvas->height() - e->y(), depth);
        Point3 world = canvas->screenToWorld(d_->mouseScreen);
        d_->surf->surface()->vertex(d_->vtxIID)->position() = d_->surf->worldToLocal(world);
        d_->surf->surface()->setChanged(true);
        
        g_pApp->getMainWindow()->getCurrentView()->updateView();
    }
}
    
void EditSubdivideCommand::paintGL(GlCanvas *c)
{

    if (d_->surf) {
        c->disable(GL_DEPTH_TEST);
        glPushMatrix();
        glMultMatrixf(d_->surf->parentTransform().data());
        glMultMatrixf(d_->surf->transform().data());
        d_->renderer.renderObject(d_->surf->surface());
        glPopMatrix();
        c->enable(GL_DEPTH_TEST);
    }
    
}
