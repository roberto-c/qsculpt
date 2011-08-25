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
#include <QtDebug>
#include <QThread>
#include <QProgressDialog>
#include "IDocument.h"
#include "ISurface.h"
#include "QSculptApp.h"
#include "QSculptWindow.h"
#include "DocumentView.h"
#include "HEdge.h"
#include "Face.h"
#include "subdivision/Subdivision.h"

typedef QHash< std::pair<int, int>, Vertex*> MidEdgeMap;

struct SubdivideCommand::Impl {
    MidEdgeMap edgeMidPoint;
    
    Impl(){}
    Impl(const Impl& cpy){}
    
    void splitEdges(ISurface& s);
    
    void addFaceMidPointVertex(ISurface& s);
    
    void createFaces(ISurface& s);
    
    void removeOldFaces(ISurface& s);
    
    void cleanUserData(ISurface& s);
    
    void smoothNormals(ISurface& s);
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

    const IDocument* doc = g_pApp->getMainWindow()->getCurrentDocument();

    if (!doc)
        return;

    _d->edgeMidPoint.clear();
    qDebug() << "Start time: " <<QDateTime::currentDateTime();

    QList<SceneNode*> list = doc->getSelectedObjects();
    foreach(SceneNode* node, list)
    {
        if (node->nodeType() != NT_Surface)
            continue;
        ISurface * obj = static_cast<SurfaceNode*>(node)->surface();
        qDebug() << "Object found";
        qDebug() << "Num faces =" << obj->getNumFaces();
        QVector<Face*> facesToDelete;
        
        // split edges in half and add a vertex per face
        _d->splitEdges(*obj);
        _d->addFaceMidPointVertex(*obj);
        
        // create new faces
        _d->createFaces(*obj);

        _d->removeOldFaces(*obj);
        
        // set to null data
        _d->cleanUserData(*obj);
        
        obj->setChanged(true);
        qDebug() << "Num Faces: " << obj->getNumFaces();
        qDebug() << "Num Vertices: " << obj->getNumVertices();
    }
    qDebug() << "End time:" << QDateTime::currentDateTime();
    dlg.setValue(100);
    g_pApp->getMainWindow()->getCurrentView()->updateView();
}

void SubdivideCommand::subdivideFace(ISurface& obj, Face& f)
{
    typedef std::pair<int, int> VertexPair;
    Point3 avg, p;
    QVector<int> vtxIndex, newFace;
    int counter = 0, nVtx = 0;
    Iterator<Edge> edgeIt = f.edgeIterator();
    
    // Add the mid point of the face to the surface object
    Vertex* faceCenter = obj.getVertex(obj.addVertex(avg / nVtx));
    // Create the faces
    edgeIt.seek(0);
    Edge *e = NULL;
    while(edgeIt.hasNext()){
        e = &edgeIt.next();
        newFace.clear();
        newFace.append(vtxIndex[counter % nVtx]);
        newFace.append(e->head()->iid());
        newFace.append(vtxIndex[(counter+1) % nVtx]);
        newFace.append(faceCenter->iid());
        obj.addFace(newFace);
        ++counter;
    }
    //qDebug() << "Num edges: " << nVtx << " Conter: " << counter;
}

void SubdivideCommand::Impl::splitEdges(ISurface& s)
{
    Iterator<Edge> edgeIt = s.edgeIterator();
    // Create new vertices at the mid point of each edge
    while(edgeIt.hasNext()){
        Edge& e = edgeIt.next();
        if (e.userData() == NULL) {
            Vector3 p = (e.tail()->position() + e.head()->position()) * 0.5f;
            Vertex * vtx = s.getVertex(s.addVertex(p));
            e.setUserData( static_cast<void*>(vtx) );
            assert(e.pair() != NULL && e.pair()->userData() == NULL);
            e.pair()->setUserData( static_cast<void*>(vtx) );
        }
    }
}

void SubdivideCommand::Impl::addFaceMidPointVertex(ISurface& s)
{
    Vector3 p;
    Iterator<Face> faceIt = s.faceIterator();
    // Create new vertices at the mid point of each edge
    while(faceIt.hasNext()){
        Face & f = faceIt.next();
        Iterator<Vertex> vtxIt = f.vertexIterator();
        p.setZero();
        int val = 0;
        while (vtxIt.hasNext()) {
            p += vtxIt.next().position();
            ++val;
        }
        p = p / val;
        Vertex * vtx = s.getVertex(s.addVertex(p));
        f.setUserData( static_cast<void*>(vtx) );
    }
}

void SubdivideCommand::Impl::createFaces(ISurface& s)
{
    int faceCounter = 0;
    QVector<int> vtxIndex, faceVtxIID;
    vtxIndex.reserve(10);
    faceVtxIID.reserve(4);
    Iterator<Face> faceIt = s.faceIterator();
    // Create new vertices at the mid point of each edge
    while(faceIt.hasNext()){
        Face & f = faceIt.next();
        // skip face if it does not contain midpoint as it is probably a new
        // face.
        if (f.userData() == NULL) {
            continue;
        }
        
        vtxIndex.clear();
        int numEdges = 0;
        faceCounter++;
        Iterator<Edge> edgeIt = f.edgeIterator();
        while (edgeIt.hasNext()) {
            // for each edge add the tail vertex and the the edge midpoint.
            Edge & e = edgeIt.next();
            vtxIndex.push_back(e.tail()->iid());
            Vertex * v = static_cast<Vertex*>(e.userData());
            assert(v != NULL);
            vtxIndex.push_back(v->iid());
            numEdges++;
        }
        Vertex * v = static_cast<Vertex*>(f.userData());
        assert(v != NULL);
        int faceCenter = v->iid();
        
        int numVerts = vtxIndex.size();
        for (int i = 0; i < numEdges; ++i) {
            faceVtxIID.clear();
            faceVtxIID.push_back(faceCenter);
            faceVtxIID.push_back(vtxIndex[((i + numEdges)*2 - 1) % numVerts]);
            faceVtxIID.push_back(vtxIndex[i*2]);
            faceVtxIID.push_back(vtxIndex[((i*2)+1) % numVerts]);
            s.addFace(faceVtxIID);
        }
    }
}

void SubdivideCommand::Impl::removeOldFaces(ISurface& s)
{
    std::vector<int> faceIndex;
    Iterator<Face> faceIt = s.faceIterator();
    while(faceIt.hasNext()){
        Face & f = faceIt.next();
        // skip face if it does not contain midpoint as it is probably a new
        // face and it should not be removed.
        if (f.userData() == NULL) {
            continue;
        }
        s.removeFace(f.iid());
    }
}

void SubdivideCommand::Impl::cleanUserData(ISurface& s)
{
    Iterator<Edge> edgeIt = s.edgeIterator();
    while(edgeIt.hasNext()){
        Edge& e = edgeIt.next();
        e.setUserData(NULL);
        e.pair()->setUserData(NULL);
    }
    Iterator<Face> faceIt = s.faceIterator();
    while(faceIt.hasNext()){
        faceIt.next().setUserData(NULL);
    }
}

void SubdivideCommand::Impl::smoothNormals(ISurface& s)
{
    Vector3 normal;
    int counter = 0;
    Iterator<Vertex> it = s.vertexIterator();
    while(it.hasNext()){
        Vertex* vtx = &it.next();
        Vector3 po = vtx->position();
        Iterator<Vertex> it2 = vtx->vertexIterator();
        counter = 0;
        while (it2.hasNext()) {
            counter++;
            it2.next();
        }
        qDebug() << "Vtx num neighbor: " << counter;
//        Vector3 v1, v2, vo;
//        normal.setZero();
//        if (it2.hasNext()) {
//            vo = v1 = it2.next().position() - po;
//            while (it2.hasNext()) {
//                v2 = it2.next().position() - po;
//                normal += v1.cross(v2);
//                counter++;
//                v1 = v2;
//            }
//            normal += v1.cross(vo);
//            counter++;
//            vtx->normal() = normal / counter;
//            vtx->normal().normalize();
//        }
    }
}
