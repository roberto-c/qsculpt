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
#include "Document.h"
#include <QFile>
#include <QTextStream>
#include "IObject3D.h"
#include "Box.h"
#include "Sphere.h"
#include "Mesh.h"

Document::Document() {

}


Document::~Document() {
    qDeleteAll(m_objectList);
    m_objectList.clear();
}

void Document::loadFile(QString fileName)
{
    qDebug("loadFile");

	IObject3D* obj = new ::Mesh();
    if (obj)
    {
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly))
        {
            qWarning("File cannot be open.");
            return;
        }
        QTextStream inputFile(&file);

        QString str;
        while(!inputFile.atEnd())
        {
            str = inputFile.readLine();
            QTextStream lineStream(&str);

            QString token;
            lineStream >> token;
            if (token == "v")
            {
                double x, y, z;
                lineStream >> x >> y >> z;
                obj->addVertex( Point3(x, y, z) );
            }
            else if (token == "f")
            {
                bool error = false;
                QVector<int> vertexIndices;
                while(!lineStream.atEnd())
                {
                    lineStream >> token;
                    QStringList values;
                    values = token.split("/");
                    int index = values[0].toInt()  - 1;
                    if (index >= 0 && index < obj->getNumVertices())
                        vertexIndices.append(index);
                    else
                    {
                        error = true;
                        qDebug("Index out of rage");
                        break;
                    }
                }
                if (!error)
                    obj->addFace( vertexIndices );
            }
        }
		obj->setChanged(true);
        addObject(IDocument::Mesh, obj);
    }
}

void Document::saveFile(QString fileName)
{
    qDebug("Save file");

    int numObjects = m_objectList.size();
    qDebug("Number of objects to save %d", numObjects);

//    if (numObjects > 0 )
//    {
//        QFile file(fileName);
//        if (!file.open(QIODevice::ReadWrite | QIODevice::Truncate))
//        {
//            qWarning("File cannot be open.");
//            return;
//        }
//        QTextStream out(&file);
//
//        const IObject3D* obj = NULL;
//        for (int i = 0; i < numObjects; ++i)
//        {
//            obj = m_objectList.at(i);
//            int numPoints = obj->getPointList().size();
//            out << "#Num vertices: " << numPoints << endl;
//            for (int j = 0; j < numPoints; ++j)
//            {
//                out << "v " << obj->getPointList().at(j).x()
//                        << " " << obj->getPointList().at(j).y()
//                        << " " << obj->getPointList().at(j).z() << endl;
//            }
//
//            int numNormals = obj->getNormalList().size();
//            out << "#Num normal: " << numNormals << endl;
//            for (int j = 0; j < numNormals; ++j)
//            {
//                out << "vn " << obj->getNormalList().at(j).x()
//                        << " " << obj->getNormalList().at(j).y()
//                        << " " << obj->getNormalList().at(j).z() << endl;
//            }
//
//            int numFaces = obj->getFaceList().size();
//            out << "#Num faces: " << numFaces << endl;
//            for (int j = 0; j < numFaces; ++j)
//            {
//                out << "f";
//                int numFacePoints = obj->getFaceList().at(j).point.size();
//                for (int k = 0; k < numFacePoints; ++k)
//                {
//                    out << " " << obj->getFaceList().at(j).point.at(k) + 1;
//                        //<< "//" << obj->getFaceList().at(j).normal.at(k) + 1;
//                }
//                out << endl;
//            }
//        }
//    }
}

void Document::addObject(ObjectType type)
{
    IObject3D* obj = NULL;

    switch(type)
    {
        case IDocument::Box:
            obj = new ::Box();
            break;
        case IDocument::Sphere:
            obj = new ::Sphere();
            break;
        case IDocument::Mesh:
            obj = new ::Mesh();
            break;
    }
    m_objectList.append( obj );
    emit changed(AddObject, obj);
}

void Document::addObject(ObjectType type, IObject3D* obj)
{
    Q_ASSERT(obj);
	
    m_objectList.append( obj );
    emit changed(AddObject, obj);
}

void Document::removeObject(int /*index*/)
{
}

IObject3D* Document::getObject(int index) const
{
    return m_objectList[index];
}

int Document::getObjectsCount() const
{
    return m_objectList.size();
}

void Document::selectObject(int /*index*/)
{
}

QList<IObject3D*> Document::getSelectedObjects() const
{
    QList<IObject3D*> selectedObjectList;
    selectedObjectList.clear();

    int count = m_objectList.size();
    for (int i = 0; i < count; ++i)
    {
        if (m_objectList[i]->isSelected())
            selectedObjectList.append(m_objectList[i]);
    }
    return selectedObjectList;
}

