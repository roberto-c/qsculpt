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

struct SubdivideCommand::Impl {
    MidEdgeMap edgeMidPoint;
    
    Impl(){}
    Impl(const Impl& cpy){}
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
        QVector<Face*> facesToDelete;
        Iterator<Face> it = obj->faceIterator();
        it.seek(0, Iter_End);
        while(it.hasPrevious()){
            Face* f = &it.previous();
            subdivideFace(*obj, *f);
            facesToDelete.append(f);
        }
        foreach(Face* f, facesToDelete) {
            obj->removeFace(f->iid());
            delete f;
        }
        facesToDelete.clear();
        obj->setChanged(true);
        qDebug() << "Num Faces: " << obj->getNumFaces();
        
        static_cast<Subdivision*>(obj)->printMemoryInfo();
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
    // Create new vertices at the mid point of each edge
    while(edgeIt.hasNext()){
        Edge& e = edgeIt.next();
        VertexPair pair(e.tail()->iid(), e.head()->iid());
        // Check if we have subdivided this edge already.
        // If so, get the vertex of that subdvision
        // If not, create the edge mid point vertex
        if (_d->edgeMidPoint.contains(pair))
        {
            Vertex* v = _d->edgeMidPoint.value(pair);
            vtxIndex.append(v->iid());
            p = v->position();
        }
        else
        {
            p = (e.tail()->position() + e.head()->position()) * 0.5f;
            Vertex* v = obj.getVertex(obj.addVertex(p));
            vtxIndex.append(v->iid());
            _d->edgeMidPoint.insert(pair, v);
        }
        // Accumulate the position. Used to compute the face mid point
        avg += p;
        nVtx++;
    }
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
