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
#include "subdividecommand.h"

#include <QtDebug>
#include <QThread>
#include <QProgressDialog>
#include "idocument.h"
#include "iobject3d.h"
#include "qsculptapp.h"
#include "qsculptwindow.h"
#include "documentview.h"

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

void SubdivideCommand::activate(bool activate)
{
	if (activate == false)
		return;

    qDebug("execute SubdivideCommand()");
    QProgressDialog dlg("Subdividing the selected object...", 0, 0, 100,
						SPAPP->getMainWindow());
    dlg.setWindowModality(Qt::WindowModal);
    dlg.setAutoReset(true);
    dlg.setAutoClose(true);
    dlg.setValue(0);

    QThread::connect(m_workerThread, SIGNAL(progress(int)),
					 &dlg, SLOT(setValue(int)));

    dlg.show();
    qDebug() << "Start time:" << QDateTime::currentDateTime();

    const IDocument* doc = SPAPP->getMainWindow()->getCurrentDocument();

    if (!doc)
        return;

    const QList<IObject3D*> selectedObjects = doc->getSelectedObjects();
    int objectCount = selectedObjects.size();

    for (int i = 0; i < objectCount; ++i)
    {
        IObject3D* obj = selectedObjects.at(i);
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
                SPAPP->processEvents();
                usleep(100 * 1000); // 100 ms
            }
        }
    }
    qDebug() << "End time:" << QDateTime::currentDateTime();
    dlg.setValue(100);
    SPAPP->getMainWindow()->getCurrentView()->updateView();
}

void SubdivideCommand::WorkerThread::subdivide(IObject3D* obj, int rbegin, int rend)
{
    Q_ASSERT(obj);
    PointContainer& pointList = obj->getPointList();
    FaceContainer& faceList = obj->getFaceList();
	EdgeContainer& edgeList = obj->getEdgeList();

    int progressValue = 0;
    Edge edge1, edge2;
    Point3D point;
    QVector<int> vertexIndex(4);
    int indexOf = -1;
	// Calculate mid face point for all faces
	for (int i = rend; i >= rbegin; --i)
	{
		obj->lock();
        const Face face = faceList.at(i);

        Vertex midFaceVertex;
        int numVertex = face.point.size();
        for (int j = 0; j < numVertex; ++j)
        {
            midFaceVertex = midFaceVertex + pointList.at(face.point.at(j)).vertex;
        }
        midFaceVertex = midFaceVertex / (float)numVertex;
        obj->addVertex(midFaceVertex/* + midNormal*/);
        int midFaceIndex = pointList.size() - 1;
		faceList[i].midPoint = midFaceIndex;
        obj->unlock();
	}

    for (int i = rend; i >= rbegin; --i)
    {
        obj->lock();
        const Face face = faceList.at(i);

        Vertex midFaceVertex;
        int numVertex = face.point.size();
        for (int j = 1; j <= numVertex; ++j)
        {
            edge1.point1 = face.point.at(j % numVertex);
            edge1.point2 = face.point.at((j+1) % numVertex);
            indexOf = edgeList.indexOf(edge1);
			if (indexOf == -1)
			{
				qDebug() << "SubdivideCommand::WorkerThread::Subdivide"
				<< "indexOf = -1 " << "edge1.p1 = " << edge1.point1
				<< " edge1.p2 = " << edge1.point2
				<< " numVertex = " << numVertex;
				break;
			}
			edge1.midPoint = edgeList.at(indexOf).midPoint;
			if (edge1.midPoint == -1)
			{
				point = pointList.at(edge1.point1).vertex
					+ pointList.at(edge1.point2).vertex;

				int faceCount = edge1.faceRef.size();
				for (int k = 0; k < faceCount; ++k)
				{
					Face f = faceList[edge1.faceRef[k]];
					point = point + pointList.at(f.midPoint).vertex;
				}
				point = point / float(faceCount + 2);
				obj->addVertex(point);
				edge1.midPoint = pointList.size() - 1;
				edgeList.setMidPointReference(indexOf, edge1.midPoint);
			}

			edge2.point1 = face.point.at((j-1) % numVertex);
            edge2.point2 = face.point.at((j) % numVertex);
            indexOf = edgeList.indexOf(edge2);
			if (indexOf == -1)
			{
				qDebug() << "SubdivideCommand::WorkerThread::Subdivide"
					<< "indexOf = -1 " << "edge2.p1 = " << edge2.point1
				<< " edge2.p2 = " << edge2.point2
				<< " numVertex = " << numVertex;
				break;
			}
			edge2.midPoint = edgeList.at(indexOf).midPoint;
			if (edge2.midPoint == -1)
			{
				point = pointList.at(edge2.point1).vertex
					+ pointList.at(edge2.point2).vertex;

				int faceCount = edge2.faceRef.size();
				for (int k = 0; k < faceCount; ++k)
				{
					Face f = faceList[edge2.faceRef[k]];
					point = point + pointList.at(f.midPoint).vertex;
				}
				point = point / float(faceCount + 2);
				obj->addVertex(point);
				edge2.midPoint = pointList.size() - 1;
				edgeList.setMidPointReference(indexOf, edge2.midPoint);
			}

            // remove this triangle from the list of faces reference of the point
            int pointIndex = face.point.at(j % numVertex);
            int faceIndex = pointList.at(pointIndex).faceRef.indexOf(i);
            if (faceIndex >= 0 )
            {
                pointList.at(pointIndex).faceRef.remove(faceIndex);
            }

            vertexIndex[0] = face.point[j % numVertex];
            vertexIndex[1] = edge1.midPoint;
            vertexIndex[2] = face.midPoint;
            vertexIndex[3] = edge2.midPoint;

            // Modify the face vertices to the new values. This is only done
            // for the first vertex. For all other vertices, a new face is added.
            // This is done to be able to maintain the original face object without
            // modifying the indices of the points and faces.

            if (j == numVertex)
            {
				obj->replaceFace(i, vertexIndex);
            }
            else
			{
                obj->addFace(vertexIndex);
			}

        }
        obj->unlock();
        int prog = (rend - rbegin - i) * 100 / (rend - rbegin);
        if (prog != progressValue)
            emit progress(prog);
        progressValue = prog;
    }

    // adjust points normals
    int pointCount = pointList.size();
    for (int i = 0; i < pointCount; ++i)
    {
        obj->lock();
        adjustPointNormal(obj, i);
        obj->unlock();
    }
    qDebug("Num Vertex: %d Num Faces: %d", pointList.size(), faceList.size());
}

void SubdivideCommand::WorkerThread::adjustPointNormal(IObject3D* obj, int index)
{
    Q_ASSERT(obj);

    PointContainer& pointList = obj->getPointList();
    FaceContainer& faceList = obj->getFaceList();

    Normal res;

    Point& p = pointList.at(index);
    int numFaces = p.faceRef.size();
    for (int i = 0; i < numFaces; i++)
    {
        res = res + computeFaceNormal(obj, p.faceRef[i]);
    }

    res = res / (float)numFaces;
    res.normalize();

    for (int i = 0; i < numFaces; i++)
    {
        Face& t = faceList[p.faceRef[i]];
        int numFacePoints = t.point.size();
        for (int j = 0; j < numFacePoints; ++j)
        {
            if (index == t.point[j])
            {
                obj->getNormalList()[t.normal[j]] = res;
            }
        }
    }
}

Point3D SubdivideCommand::WorkerThread::computeFaceNormal(const IObject3D* obj, int index)
{
    Q_ASSERT(obj);

    const FaceContainer& faceList = obj->getFaceList();

    return faceList.size() > index ? computeFaceNormal(obj, faceList[index]) : Point3D();
}

Point3D SubdivideCommand::WorkerThread::computeFaceNormal(const IObject3D* obj, const Face &face)
{
    Q_ASSERT(obj);

    const PointContainer& pointList = obj->getPointList();

    int lastPoint = face.point.size() - 1;
    Point3D v1 = pointList.at(face.point[1]).vertex - pointList.at(face.point[0]).vertex;
    Point3D v2 = pointList.at(face.point[lastPoint]).vertex - pointList.at(face.point[0]).vertex;

    Point3D res = v1.crossProduct( v2);
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

