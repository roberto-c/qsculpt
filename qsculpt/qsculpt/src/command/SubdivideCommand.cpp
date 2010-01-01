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
#include "SubdivideCommand.h"
#include <QtDebug>
#include <QThread>
#include <QProgressDialog>
#include "IDocument.h"
#include "IObject3D.h"
#include "QSculptApp.h"
#include "QSculptWindow.h"
#include "DocumentView.h"

SubdivideCommand::SubdivideCommand()
 : CommandBase("Subdivide"),
    m_workerThread(new WorkerThread)
{
}

SubdivideCommand::SubdivideCommand(const SubdivideCommand& cpy)
: CommandBase(cpy),
	m_workerThread(new WorkerThread)
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

    QThread::connect(m_workerThread, SIGNAL(progress(int)),
					 &dlg, SLOT(setValue(int)));

    dlg.show();
    qDebug() << "Start time:" << QDateTime::currentDateTime();

    const IDocument* doc = g_pApp->getMainWindow()->getCurrentDocument();

    if (!doc)
        return;

    const QList<IObject3D*> selectedObjects = doc->getSelectedObjects();
    int objectCount = selectedObjects.size();

    for (int faceIndex = 0; faceIndex < objectCount; ++faceIndex)
    {
        IObject3D* obj = selectedObjects.at(faceIndex);
        if (obj)
        {
            obj->lock();
            PointContainer& p = obj->getPointList();
            p.reserve(p.size()*4);
            obj->unlock();
            int faceCount = obj->getFaceList().size();
            m_workerThread->setObject3D(obj);
            m_workerThread->setRangeBegin(0);
            m_workerThread->setRangeEnd( faceCount  - 1);
            m_workerThread->start();
            while(m_workerThread->isRunning())
            {
                g_pApp->processEvents();
                usleep(100 * 1000); // 100 ms
            }
			obj->setChanged(true);
        }
    }
    qDebug() << "End time:" << QDateTime::currentDateTime();
    dlg.setValue(100);
    g_pApp->getMainWindow()->getCurrentView()->updateView();
}

void SubdivideCommand::WorkerThread::subdivide(IObject3D* obj, int rbegin, int rend)
{	
	Q_ASSERT(obj);
	
	int progressValue = 0;
	
	obj->lock();
	for (int faceIndex = rend; faceIndex >= rbegin; --faceIndex)
	{		
		subdivideFace(obj, faceIndex);
		
        int prog = (rend - rbegin - faceIndex) * 100 / (rend - rbegin);
        if (prog != progressValue)
            emit progress(prog);
        progressValue = prog;
	}
	obj->unlock();
	
	//qDebug() << "subdivide: Adjust normals"; 
	
    // adjust points normals
    int pointCount = obj->getPointList().size();
	obj->lock();
    for (int faceIndex = 0; faceIndex < pointCount; ++faceIndex)
    {
        adjustPointNormal(obj, faceIndex);
    }
	obj->unlock();
    qDebug("subdivide: Num Vertex: %d Num Faces: %d", 
		   obj->getPointList().size(), 
		   obj->getFaceList().size());
}

void SubdivideCommand::WorkerThread::subdivideFace(IObject3D* obj, int faceIndex)
{
#define POINT_LIST() (obj->getPointList())
#define FACE_LIST() (obj->getFaceList())
#define EDGE_LIST() (obj->getEdgeList())
	
	Q_ASSERT(obj);
	Q_ASSERT(faceIndex >= 0 && faceIndex < obj->getFaceList().size());
	Point3 vertex;
	QVector<int> vtxIndices(4);
	
	Face f = FACE_LIST().at(faceIndex);
	int numFaceVertices = f.point.size();
	
	// Find the vertex at the center of the polygon 
	Point3 midVertex;
	for (int j = 0; j < numFaceVertices; ++j)
	{
		midVertex = midVertex + POINT_LIST().at(f.point.at(j));
	}
	midVertex = midVertex / (float)numFaceVertices;
	
	// Add the new vertex to the mesh
	int midVertexIndex = obj->addVertex(midVertex);
	FACE_LIST()[faceIndex].midPoint = midVertexIndex;
	
	int edgeIndex = -1;
	QVector<Edge> edgeList(numFaceVertices);
	Edge edge;
	for (int j = 0; j < numFaceVertices; ++j)
	{
		edgeIndex = EDGE_LIST().indexOf(
										Edge( f.point[j], f.point[(j+1)%numFaceVertices] )
										);
		if (edgeIndex == -1)
		{
			qDebug() << "subdivide: Edge not found";
			obj->unlock();
			return;
		}
		edge = EDGE_LIST().at(edgeIndex);
		edgeList[j] = edge;
		
		// If the edge has no mid point assigned, calculate the mid point
		// and add it to the mesh
		if (edge.midPoint == -1)
		{
			midVertex = POINT_LIST().at(edge.point1) + POINT_LIST().at(edge.point2);
			midVertex = midVertex / 2.0f;
			midVertexIndex = obj->addVertex(midVertex);
			if (midVertexIndex == -1)
			{
				qDebug() << "subdivide: Edge not found";
				obj->unlock();
				return;
			}
			EDGE_LIST().setMidPointReference(edgeIndex, midVertexIndex);
			edgeList[j] = EDGE_LIST().at(edgeIndex);
		}
	}
	f = FACE_LIST().at(faceIndex);
	vtxIndices[0] = edgeList[3].commonVertex(edgeList[0]);
	vtxIndices[1] = edgeList[0].midPoint;
	vtxIndices[2] = f.midPoint;
	vtxIndices[3] = edgeList[3].midPoint;
	obj->addFace(vtxIndices);
	vtxIndices[0] = edgeList[0].midPoint;
	vtxIndices[1] = edgeList[0].commonVertex(edgeList[1]);
	vtxIndices[2] = edgeList[1].midPoint;
	vtxIndices[3] = f.midPoint;
	obj->addFace(vtxIndices);
	vtxIndices[0] = f.midPoint;
	vtxIndices[1] = edgeList[1].midPoint;
	vtxIndices[2] = edgeList[1].commonVertex(edgeList[2]);
	vtxIndices[3] = edgeList[2].midPoint;
	obj->addFace(vtxIndices);
	vtxIndices[0] = edgeList[3].midPoint;
	vtxIndices[1] = f.midPoint;
	vtxIndices[2] = edgeList[2].midPoint;
	vtxIndices[3] = edgeList[2].commonVertex(edgeList[3]);
	obj->replaceFace(faceIndex, vtxIndices);
	FACE_LIST()[faceIndex].midPoint = -1;
}

void SubdivideCommand::WorkerThread::adjustPointNormal(IObject3D* obj, int index)
{
    Q_ASSERT(obj);

    PointContainer& pointList = obj->getPointList();
//    FaceContainer& faceList = obj->getFaceList();

    Vector3 res;

    //Vertex& p = pointList.at(index);
    int numFaces = pointList.getFaceReference(index).size();
    for (int faceIndex = 0; faceIndex < numFaces; faceIndex++)
    {
        res = res + computeFaceNormal(obj, pointList.getFaceReference(index).at(faceIndex));
    }

    res = res / (float)numFaces;
    res.normalize();
    obj->getNormalList()[index] = res;
	
//    for (int i = 0; i < numFaces; i++)
//    {
//        Face& t = faceList[pointList.getFaceReference(index).at(i)];
//        int numFacePoints = t.point.size();
//        for (int j = 0; j < numFacePoints; ++j)
//        {
//            if (index == t.point[j])
//            {
//                obj->getNormalList()[t.point[j]] = res;
//            }
//        }
//    }
}

Point3 SubdivideCommand::WorkerThread::computeFaceNormal(const IObject3D* obj, int index)
{
    Q_ASSERT(obj);

    const FaceContainer& faceList = obj->getFaceList();

    return faceList.size() > index ? computeFaceNormal(obj, faceList[index]) : Point3();
}

Point3 SubdivideCommand::WorkerThread::computeFaceNormal(const IObject3D* obj, const Face &face)
{
    Q_ASSERT(obj);

    const PointContainer& pointList = obj->getPointList();

    int lastPoint = face.point.size() - 1;
    Point3 v1 = pointList.at(face.point[1]) - pointList.at(face.point[0]);
    Point3 v2 = pointList.at(face.point[lastPoint]) - pointList.at(face.point[0]);

    Point3 res = v1.cross( v2);
    res.normalize();

    return res;
}

void SubdivideCommand::WorkerThread::run() {
    qDebug("Running...");
    subdivide(m_obj, m_rbegin, m_rend);
    qDebug("Ending thread");
}

void SubdivideCommand::WorkerThread::setRangeBegin(unsigned int value)
{
    m_rbegin = value;
}

void SubdivideCommand::WorkerThread::setRangeEnd(unsigned int value)
{
    m_rend = value;
}

void SubdivideCommand::WorkerThread::setObject3D(IObject3D* obj)
{
    m_obj = obj;
}

