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
    typedef std::shared_ptr<ISurface> shared_ptr;
    typedef std::weak_ptr<ISurface> weak_ptr;
    typedef std::unique_ptr<ISurface> Ptr;
    
private:
    Document::shared_ptr _doc;

public:
    SurfaceIterator(Document::weak_ptr surface) ;
    
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
    virtual shared_ptr next();

    /**
     * Returns the next element and advance the iterator by one.
     */
    virtual const shared_ptr next() const;

    /**
     * Returns the previous elements and move the iterator one position
     * backwards.
     */
    virtual shared_ptr previous();

    /**
     * Returns the previous elements and move the iterator one position
     * backwards.
     */
    virtual const shared_ptr previous() const;

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
    typedef std::shared_ptr<SceneNode>      shared_ptr;
    typedef std::weak_ptr<SceneNode>        weak_ptr;
    typedef std::weak_ptr<const SceneNode>  const_weak_ptr;
    typedef std::unique_ptr<SceneNode>      Ptr;
    
private:
    Document::const_shared_ptr  _doc;
    Scene::shared_ptr            _scene;
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
    virtual shared_ptr next();
    
    /**
     * Returns the next element and advance the iterator by one.
     */
    virtual const shared_ptr next() const;
    
    /**
     * Returns the previous elements and move the iterator one position
     * backwards.
     */
    virtual shared_ptr previous();
    
    /**
     * Returns the previous elements and move the iterator one position
     * backwards.
     */
    virtual const shared_ptr previous() const;
    
    /**
     * Set the current position to pos relative to origin.
     *
     * @param pos number of elements to jump relative to origin
     * @param origin states the reference to jump.
     */
    virtual bool seek(int pos, IteratorOrigin origin) const;
};


struct Document::Impl {
    Scene::shared_ptr       scene;
    SceneNode::shared_ptr   rootNode;
    
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

QList<SceneNode::weak_ptr> Document::getSelectedObjects() const
{
    QList<SceneNode::weak_ptr> selectedObjectList;
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

SceneNode::weak_ptr Document::rootNode()
{
    return _d->rootNode;
}

SceneNode::weak_ptr Document::rootNode() const
{
    return _d->rootNode;
}


Scene::weak_ptr Document::scene()
{
    return _d->scene;
}

Scene::weak_ptr Document::scene() const
{
    return _d->scene;
}

int Document::columnCount(const QModelIndex & parent) const
{
    return 1;
}

QVariant Document::data (const QModelIndex & index, 
                           int role ) const
{
    if (role == Qt::DisplayRole) {
        QVariant data(QString("Name"));
        return data;
    }
    return QVariant();
}

QModelIndex Document::index (int row, 
                               int column, 
                               const QModelIndex & parent ) const
{
    assert(_d && _d->scene);
    
    SceneNode::shared_ptr p = NULL;
    if (parent.isValid()) {
        p = _d->scene->findByIID(parent.internalId());
    }
    if (p) {
        p = p->item(row).lock();
    } else if (_d->scene) {
        auto ptr = _d->scene->item(row);
        p = ptr.lock();
    }
    if (!p) {
        //return createIndex(-1, -1, 0);
        return QModelIndex();
    }
    return createIndex(row, 0, p->iid());
}

QModelIndex Document::parent ( const QModelIndex & index ) const
{
    assert(_d && _d->scene);
    
    if (!index.isValid()) {
        //throw std::runtime_error("Invalid index");
        return QModelIndex();
    }
    
    SceneNode::shared_ptr p = _d->scene->findByIID(index.internalId());
    int row = -1;
    
    if (p) {
        size_t n = 0;
        row = p->itemIndex(p, &n) ? n : -1;
    } else {
        if (_d->scene) {
            p = _d->scene;
            size_t n = 0;
            row = p->itemIndex(p, &n) ? n : -1;
        }
    }
    if (!p) {
        //return createIndex(-1, -1, 0);
        return QModelIndex();
    }
    return createIndex(row, 0, p->iid());
}

int Document::rowCount ( const QModelIndex & parent ) const
{
    assert(_d && _d->scene);
    
    if (parent.isValid()) {
        SceneNode::shared_ptr p = _d->scene->findByIID(parent.internalId());
        if (p) {
            return p->count();
        }
    }
    if (_d->scene) {
        return _d->scene->count();
    }
    return 0;
}

SceneNode::shared_ptr Document::findItem(uint iid) 
{
    SceneNode::shared_ptr p;
    return _d->scene->findByIID(iid);
}

bool Document::insertRow ( int row, const QModelIndex & parent )
{
    return false;
}

void Document::addItem(SceneNode::shared_ptr node, 
                     const QModelIndex & parent)
{
    assert(_d && _d->scene);
    
    int rows = rowCount(parent);
    beginInsertRows(parent, rows, rows);
    
    SceneNode::shared_ptr p;
    if (parent.isValid()) {
        p = _d->scene->findByIID(parent.internalId());
    }
    
    if (!p) {
        p = _d->scene;
    }
    if (p) {
        p->add(node);
    } else {
        qDebug() << "failed to add node.";
    }
    endInsertRows();
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
SurfaceIterator::SurfaceIterator(Document::weak_ptr doc)
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

ISurface::shared_ptr SurfaceIterator::next()
{
    //NOT_IMPLEMENTED
    throw std::runtime_error("Not implemented");
}

const ISurface::shared_ptr SurfaceIterator::next() const
{
    //NOT_IMPLEMENTED
    throw std::runtime_error("Not implemented");
//    Face *f = _index.value();
//    assert(f);
//    ++_index;
//    return *f;
}

ISurface::shared_ptr SurfaceIterator::previous()
{
    throw std::runtime_error("Not implemented");
//    --_index;
//    return *_index.value();
}

const ISurface::shared_ptr SurfaceIterator::previous() const
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

SceneNode::shared_ptr SceneIterator::next()
{
    //NOT_IMPLEMENTED
    //throw std::runtime_error("Not implemented");
    SceneNode::weak_ptr n = _scene->item(_current);
    _current++;
    return n.lock();
}

const SceneNode::shared_ptr SceneIterator::next() const
{
    //NOT_IMPLEMENTED
    //throw std::runtime_error("Not implemented");
    //    Face *f = _index.value();
    //    assert(f);
    //    ++_index;
    //    return *f;
    SceneNode::weak_ptr n = _scene->item(_current);
    _current++;
    return n.lock();
}

SceneNode::shared_ptr SceneIterator::previous()
{
    throw std::runtime_error("Not implemented");
    //    --_index;
    //    return *_index.value();
}

const SceneNode::shared_ptr SceneIterator::previous() const
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
