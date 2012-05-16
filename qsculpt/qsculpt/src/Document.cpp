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
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include "ISurface.h"
#include "subdivision/Box.h"
#include "subdivision/Sphere.h"
#include "Mesh.h"
#include "Quad.h"
#include <stdexcept>
#include "SceneNode.h"
#include "Scene.h"

static QAtomicInt NEXT_ID(0);

class SurfaceIterator : public IIterator<ISurface>
{
public:
    typedef std::shared_ptr<ISurface> SharedPtr;
    typedef std::weak_ptr<ISurface> WeakPtr;
    typedef std::unique_ptr<ISurface> Ptr;
    
private:
    Document::SharedPtr _doc;

public:
    SurfaceIterator(Document::WeakPtr surface) ;
    
    ~SurfaceIterator() ;

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
    virtual SharedPtr next();

    /**
     * Returns the next element and advance the iterator by one.
     */
    virtual const SharedPtr next() const;

    /**
     * Returns the previous elements and move the iterator one position
     * backwards.
     */
    virtual SharedPtr previous();

    /**
     * Returns the previous elements and move the iterator one position
     * backwards.
     */
    virtual const SharedPtr previous() const;

    /**
     * Set the current position to pos relative to origin.
     *
     * @param pos number of elements to jump relative to origin
     * @param origin states the reference to jump.
     */
    virtual bool seek(int pos, IteratorOrigin origin) const;
};

class SceneIterator : public IIterator<SceneNode>
{
public:
    typedef std::shared_ptr<SceneNode>      SharedPtr;
    typedef std::weak_ptr<SceneNode>        WeakPtr;
    typedef std::weak_ptr<const SceneNode>  const_weak_ptr;
    typedef std::unique_ptr<SceneNode>      Ptr;
    
private:
    Document::const_shared_ptr  _doc;
    Scene::SharedPtr            _scene;
    mutable int                 _current;
    
public:
    SceneIterator(const Document::const_weak_ptr & doc) ;
    
    ~SceneIterator() ;
    
    /**
     * Function to copy iterators of the same type.
     */
    virtual IIterator<SceneNode>* clone() const;
    
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
    virtual SharedPtr next();
    
    /**
     * Returns the next element and advance the iterator by one.
     */
    virtual const SharedPtr next() const;
    
    /**
     * Returns the previous elements and move the iterator one position
     * backwards.
     */
    virtual SharedPtr previous();
    
    /**
     * Returns the previous elements and move the iterator one position
     * backwards.
     */
    virtual const SharedPtr previous() const;
    
    /**
     * Set the current position to pos relative to origin.
     *
     * @param pos number of elements to jump relative to origin
     * @param origin states the reference to jump.
     */
    virtual bool seek(int pos, IteratorOrigin origin) const;
};


struct Document::Impl {
    Scene::SharedPtr       scene;
    SceneNode::SharedPtr   rootNode;
    
    Impl() :scene(new Scene),
        rootNode(new SceneNode("Scene")) {
        
    }
};

Document::Document() : _d(new Impl)
{
}


Document::~Document() 
{    
}

void Document::loadFile(const QString& fileName)
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
                QVector<size_t> vertexIndices;
                while(!lineStream.atEnd())
                {
                    lineStream >> token;
                    QStringList values;
                    values = token.split("/");
                    int index = values[0].toInt()  - 1;
                    if (index >= 0 && index < obj->numVertices())
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
        //        addObject(IDocument::Mesh, obj);
    }
}

void Document::saveFile(const QString& fileName)
{
    Q_UNUSED(fileName);
    qDebug("Save file");

    //int numObjects = m_objectList.size();
    //qDebug("Number of objects to save %d", numObjects);

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



void Document::selectObject(int iid)
{
//    ISurface *s = getObject(iid);
//    if (s)
//    {
//        s->setSelected(true);
//    }
}

QList<SceneNode::WeakPtr> Document::getSelectedObjects() const
{
    QList<SceneNode::WeakPtr> selectedObjectList;
    selectedObjectList.clear();

    Iterator<SceneNode> it = constSceneIterator();
    while (it.hasNext()) {
        auto n = it.next();
        if (n->isSelected()) {
            selectedObjectList.append(n);
        }
//        if (n->nodeType() == NT_Surface)
//        {
//            ISurface *s = static_cast<SurfaceNode*>(n)->surface();
//            if (s->isSelected())
//            {
//                selectedObjectList.append(n);
//            }
//        }
    }
    return selectedObjectList;
}

SceneNode::WeakPtr Document::rootNode()
{
    return _d->rootNode;
}

SceneNode::WeakPtr Document::rootNode() const
{
    return _d->rootNode;
}


Scene::WeakPtr Document::scene()
{
    return _d->scene;
}

Scene::WeakPtr Document::scene() const
{
    return _d->scene;
}


Iterator<SceneNode> Document::sceneIterator()
{
    auto ptr = std::dynamic_pointer_cast<Document>(shared_from_this());
    return Iterator<SceneNode>(new SceneIterator(ptr));
}

Iterator<SceneNode> Document::constSceneIterator() const
{
    auto ptr = std::dynamic_pointer_cast<const Document>(shared_from_this());
    return Iterator<SceneNode>(new SceneIterator(ptr));
}

Iterator<ISurface> Document::surfaceIterator()
{
    auto ptr = std::dynamic_pointer_cast<Document>(shared_from_this());
    return Iterator<ISurface>(new SurfaceIterator(ptr));
}


/// Iterator definition
SurfaceIterator::SurfaceIterator(Document::WeakPtr doc)
    :	_doc(doc)
{
    assert(!doc.expired());
}

SurfaceIterator::~SurfaceIterator()
{
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

ISurface::SharedPtr SurfaceIterator::next()
{
    //NOT_IMPLEMENTED
    throw std::runtime_error("Not implemented");
}

const ISurface::SharedPtr SurfaceIterator::next() const
{
    //NOT_IMPLEMENTED
    throw std::runtime_error("Not implemented");
//    Face *f = _index.value();
//    assert(f);
//    ++_index;
//    return *f;
}

ISurface::SharedPtr SurfaceIterator::previous()
{
    throw std::runtime_error("Not implemented");
//    --_index;
//    return *_index.value();
}

const ISurface::SharedPtr SurfaceIterator::previous() const
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


SceneIterator::SceneIterator(const Document::const_weak_ptr & doc)
:	_doc(doc),
    _current(0)
{
    assert(!doc.expired());
    _scene = _doc->scene().lock();
}

SceneIterator::~SceneIterator()
{
    _doc = NULL;
    _scene.reset();
}

IIterator<SceneNode>* SceneIterator::clone() const
{
    SceneIterator *it = new SceneIterator(_doc);
    return it;
}

bool SceneIterator::hasNext() const
{
    //    int n = _surface->getNumFaces();
    //    return n > 0 && _index != _surface->_faces->end();
    return _current < _scene->count();
}

bool SceneIterator::hasPrevious() const
{
    //    int n = _surface->getNumFaces();
    //    return n > 0 &&
    //            (_index == _surface->_faces->end() ||
    //             _index != _surface->_faces->begin());
    return false;
}

SceneNode::SharedPtr SceneIterator::next()
{
    //NOT_IMPLEMENTED
    //throw std::runtime_error("Not implemented");
    SceneNode::WeakPtr n = _scene->item(_current);
    _current++;
    return n.lock();
}

const SceneNode::SharedPtr SceneIterator::next() const
{
    //NOT_IMPLEMENTED
    //throw std::runtime_error("Not implemented");
    //    Face *f = _index.value();
    //    assert(f);
    //    ++_index;
    //    return *f;
    SceneNode::WeakPtr n = _scene->item(_current);
    _current++;
    return n.lock();
}

SceneNode::SharedPtr SceneIterator::previous()
{
    throw std::runtime_error("Not implemented");
    //    --_index;
    //    return *_index.value();
}

const SceneNode::SharedPtr SceneIterator::previous() const
{
    throw std::runtime_error("Not implemented");
    //    --_index;
    //    return *_index.value();
}

bool SceneIterator::seek(int /*pos*/, IteratorOrigin /*origin*/) const
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
