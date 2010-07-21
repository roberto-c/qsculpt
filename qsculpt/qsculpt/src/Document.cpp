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
#include "ISurface.h"
#include "Box.h"
#include "Sphere.h"
#include "Mesh.h"
#include "Quad.h"
#include <stdexcept>


class SurfaceIterator : public IIterator<ISurface>
{
    Document* _doc;

public:
    SurfaceIterator(Document* surface) ;

    /**
     * Function to copy iterators of the same type.
     */
    virtual IIterator<ISurface>* clone() const;

    /**
     * Return true if the iterator has more elements (i.e. it is not at the
     * end)
     */
    virtual bool hasNext() const;

    /**
     * Returns true if the iterator is not at the beginning of the iteration
     */
    virtual bool hasPrevious() const;

    /**
     * Returns the next element and advance the iterator by one.
     */
    virtual ISurface & next();

    /**
     * Returns the next element and advance the iterator by one.
     */
    virtual const ISurface & next() const;

    /**
     * Returns the previous elements and move the iterator one position
     * backwards.
     */
    virtual ISurface & previous();

    /**
     * Returns the previous elements and move the iterator one position
     * backwards.
     */
    virtual const ISurface & previous() const;

    /**
     * Set the current position to pos relative to origin.
     *
     * @param pos number of elements to jump relative to origin
     * @param origin states the reference to jump.
     */
    virtual bool seek(int pos, IteratorOrigin origin) const;
};

Document::Document() {

}


Document::~Document() {
    qDeleteAll(m_objectList);
    m_objectList.clear();
}

void Document::loadFile(QString fileName)
{
    qDebug("loadFile");

	ISurface* obj = new ::Mesh();
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
    Q_UNUSED(fileName);
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
    ISurface* obj = NULL;

    switch(type)
    {
        case IDocument::Box:
            obj = new ::Quad();
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

void Document::addObject(ObjectType type, ISurface* obj)
{
    Q_ASSERT(obj);
    Q_UNUSED(type);
    m_objectList.append( obj );
    emit changed(AddObject, obj);
}

void Document::removeObject(int /*index*/)
{
}

ISurface* Document::getObject(int index) const
{
    return m_objectList[index];
}

int Document::getObjectsCount() const
{
    return m_objectList.size();
}

void Document::selectObject(int index)
{
    if (index < m_objectList.size())
    {
        m_objectList[index]->setSelected(true);
    }
}

QList<ISurface*> Document::getSelectedObjects() const
{
    QList<ISurface*> selectedObjectList;
    selectedObjectList.clear();

    int count = m_objectList.size();
    for (int i = 0; i < count; ++i)
    {
        if (m_objectList[i]->isSelected())
            selectedObjectList.append(m_objectList[i]);
    }
    return selectedObjectList;
}

Iterator<ISurface> Document::surfaceIterator()
{
    return Iterator<ISurface>(new SurfaceIterator(this));
}


/// Iterator definition
SurfaceIterator::SurfaceIterator(Document* doc)
    :	_doc(doc)
{
    assert(doc);
}

IIterator<ISurface>* SurfaceIterator::clone() const
{
    SurfaceIterator *it = new SurfaceIterator(_doc);
    return it;
}

bool SurfaceIterator::hasNext() const
{
//    int n = _surface->getNumFaces();
//    return n > 0 && _index != _surface->_faces->end();
    return false;
}

bool SurfaceIterator::hasPrevious() const
{
//    int n = _surface->getNumFaces();
//    return n > 0 &&
//            (_index == _surface->_faces->end() ||
//             _index != _surface->_faces->begin());
    return false;
}

ISurface & SurfaceIterator::next()
{
    //NOT_IMPLEMENTED
    throw std::runtime_error("Not implemented");
}

const ISurface & SurfaceIterator::next() const
{
    //NOT_IMPLEMENTED
    throw std::runtime_error("Not implemented");
//    Face *f = _index.value();
//    assert(f);
//    ++_index;
//    return *f;
}

ISurface & SurfaceIterator::previous()
{
    throw std::runtime_error("Not implemented");
//    --_index;
//    return *_index.value();
}

const ISurface & SurfaceIterator::previous() const
{
    throw std::runtime_error("Not implemented");
//    --_index;
//    return *_index.value();
}

bool SurfaceIterator::seek(int /*pos*/, IteratorOrigin /*origin*/) const
{
//    switch(origin)
//    {
//    case Iter_Current:
//        // nothing
//        break;
//    case Iter_End:
//        _index = _surface->_faces->end();
//        break;

//    case Iter_Start:
//    default:
//        _index = _surface->_faces->begin();
//        break;
//    }

//    if (pos > 0) {
//        while(--pos && _index != _surface->_faces->end()) {
//            ++_index;
//        }
//        if (_index == _surface->_faces->end())
//            return false;
//    } else if (pos < 0 ) {
//        while(++pos && _index != _surface->_faces->end()) {
//            --_index;
//        }
//        if (_index == _surface->_faces->end())
//            return false;
//    }
    return false;
}

