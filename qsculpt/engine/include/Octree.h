/***************************************************************************
*   Copyright (C) 2007 by Juan Roberto Cabral Flores   *
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
#ifndef OCTREE_H
#define OCTREE_H

#include <QtDebug>
#include <QVector>
#include "Point3D.h"
#include "ICollection.h"

template<typename T> class Octree;

template< typename D >
class OctreeNode {
    static const int MaxVertex = 4;
    static const int MaxDepth = 50;
public:
    OctreeNode(Octree<D>* octree);
    OctreeNode(OctreeNode<D>* parent, int depth = 0);
    ~OctreeNode();

    inline void setMinimumCoords(const Point3& v);
    inline Point3 getMinimumCoords();
    inline void setMaximumCoords(const Point3& v);
    inline Point3 getMaximumCoords();
    bool findClosest(const Point3& v, D* data) ;
    int indexOf(const D& data);
    inline bool contains(const D& data);
    bool add(const D& data) ;
    bool remove(const D&);
    void clear();

    QString toString();

protected:
    void doPartition();

    bool isInVolume(const D& v);

private:
    OctreeNode(const OctreeNode<D>&);
    OctreeNode& operator=(const OctreeNode<D>&);

    Octree<D>*              m_octree;           // Pointer to owner octree
    OctreeNode<D>*          m_parent;           // Pointer to parent node
    QVector<OctreeNode<D>*> m_childrenNodes;    // Children nodes.
    QVector<int>            m_dataIndices;  //
    Point3                  m_minimumCoords;
    Point3                  m_maximumCoords;
    bool                    m_hasChildren;
    int                     m_depth;
};

template< typename Type>
class Octree : public data::ICollection<Type>
{
public:
    Octree() {
        m_rootNode = new OctreeNode<Type>(this);
        m_rootNode->setMinimumCoords(Point3(-1024, -1024, -1024));
        m_rootNode->setMaximumCoords(Point3(1024, 1024, 1024));
    }

    Octree(const Octree<Type>& cpy) {
        m_rootNode = new OctreeNode<Type>(this);
        m_rootNode->setMinimumCoords(Point3(-1024, -1024, -1024));
        m_rootNode->setMaximumCoords(Point3(1024, 1024, 1024));

        foreach (Point3 v, m_data) {
            append(v);
        }
    }

    ~Octree() {
        delete m_rootNode;
    }

    inline Type* getData() {
        return m_data.data();
    }

    inline Type& operator[](int index) {
        return m_data[index];
    }

    inline const Type& operator[](int index) const {
        return m_data[index];
    }

    inline const Type& at(int index) const {
        return m_data.at(index);
    }

    inline bool contains(const Type& v) const {
        return m_rootNode->contains(v);
    }

    inline bool append(const Type& v) {
        //qDebug("Octree::add");
        if (!m_rootNode->add(v))
        {
            qDebug("%s", qPrintable("Octree::add : failed: Point: " + ::toString(v)));
            return false;
        }
        return true;
    }

    inline void remove(const Type& v) {
        Q_UNUSED(v);
    }

    inline void remove(int index) {
        Q_UNUSED(index);
    }

    inline void clear() {
        m_rootNode->clear();
        m_data.clear();
    }

    inline int size() const {
        return m_data.size();
    }

    inline int findClosest(const Point3& v) const {
        Type data;
        if (m_rootNode->findClosest(v, &data)) {
            return m_rootNode->indexOf(data);
        }
        return -1;
    }

    inline void reserve(int _size) {
        m_data.reserve(_size);
    }

    inline QString toString() {
        return m_rootNode->toString();
    }

    inline QVector<Type> toQVector() {
        return m_data;
    }

    inline const QVector<Type> toQVector() const {
        return m_data;
    }

private:


    // Do not allow coping semantincs
    Octree& operator=(const Octree<Type>&);

    OctreeNode<Type>*  m_rootNode;
    QVector<Type>      m_data;

};



template<typename D>
OctreeNode< D >::OctreeNode(Octree<D>* octree)
    : m_octree(octree),
    m_parent(NULL),
    m_hasChildren(false),
    m_depth (0)
{
    //    m_minimumCoords = Vertex::null();
    //    m_maximumCoords = Vertex::null();
}

template<typename D>
OctreeNode<D>::OctreeNode(OctreeNode<D>* parent, int depth)
    : m_parent(parent),
    m_hasChildren(false),
    m_depth (depth)
{
    m_octree = m_parent != NULL ? m_parent->m_octree : NULL;
    //    m_minimumCoords = Vertex::null();
    //    m_maximumCoords = Vertex::null();
}

template<typename D>
OctreeNode<D>::~OctreeNode()
{
    clear();
}

template<typename D>
inline void OctreeNode<D>::setMinimumCoords(const Point3& v)
{
    m_minimumCoords = v;
}

template<typename D>
inline Point3 OctreeNode<D>::getMinimumCoords()
{
    return m_minimumCoords;
}

template<typename D>
inline void OctreeNode<D>::setMaximumCoords(const Point3& v)
{
    m_maximumCoords = v;
}

template<typename D>
inline Point3 OctreeNode<D>::getMaximumCoords()
{
    return m_maximumCoords;
}

template<typename D>
void OctreeNode<D>::clear()
{
    int size = m_childrenNodes.size();
    OctreeNode<D>* d = NULL;
    for(int i = 0; i < size; ++i) {
        d = m_childrenNodes[i];
        if (d) {
            d->clear();
            delete d;
        }
    }
    m_childrenNodes.clear();
    m_dataIndices.clear();
}

template<typename D>
bool OctreeNode<D>::findClosest(const Point3& v, D* data)
{
    if (data == NULL)
        return false;

    //if (!isInVolume(v))
    //  return Vertex::null();;

    if (m_hasChildren) {
        bool res;
        foreach(OctreeNode<D>* node, m_childrenNodes) {
            res = node->findClosest(v, data);
            if (res) {
                return res;
            }
        }
    }
    else {
        if (m_dataIndices.isEmpty())
            return false;

        double distance = 0.0, minDistance = -1.0;
        int i = 0, indexOf = 0, dataSize = m_dataIndices.size();
        qDebug("OctreeNode: dataSize = %d", dataSize);
        D tmp;
        do {
            tmp = m_octree->toQVector().at(m_dataIndices[i]);
            distance = (v - tmp).length();
            if (distance <= minDistance || minDistance < 0.0) {
                minDistance = distance;
                indexOf = m_dataIndices[i];
            }
            ++i;
        } while(i < dataSize);
        *data = m_octree->toQVector().at(indexOf);
        return true;
    }
    return false;
}

template<typename D>
int OctreeNode<D>::indexOf(const D& data)
{
    int index = -1;

    //if (data == D::null()) {
    //  qDebug("OctreeNode::indexOf: Vertex cannot be null()");
    //  return false;
    //}

    if (!isInVolume(data))
        return false;

    if (m_hasChildren) {
        if (m_childrenNodes.isEmpty())
            return index;

        int size = m_childrenNodes.size();
        for (int i = 0; i < size; ++i) {
            index = m_childrenNodes[i]->indexOf(data);
            if (index)
                break;
        }
    }
    else {
        if (m_dataIndices.isEmpty())
            return index;

        int size = m_dataIndices.size();
        for (int i = 0; i < size; ++i) {
            if (m_octree->at(m_dataIndices[i]) == data) {
                index = m_dataIndices[i];
                break;
            }
        }
    }
    return index;
}

template<typename D>
inline bool OctreeNode<D>::contains(const D& data)
{
    return indexOf(data) != -1;
}

template<typename D>
bool OctreeNode<D>::add(const D& data)
{
    // TODO: This method doesn't work, crash!!!
    if ( !isInVolume(data) )
        return false;

    if ( m_hasChildren ) {
        foreach(OctreeNode<D>* n, m_childrenNodes) {
            if ( n->add(data) )
                return true;
        }
    }
    else {
        if ( m_dataIndices.size() < MaxVertex || m_depth >= MaxDepth ) {
            m_octree->m_data.append(data);
            int index = m_octree->m_data.size() - 1;
            m_dataIndices.append(index);
            return true;
        }
        else {
            doPartition();
            foreach(OctreeNode<D>* n, m_childrenNodes) {
                if ( n->add(data) )
                    return true;
            }
        }
    }
    return false;
}

template<typename D>
void OctreeNode<D>::doPartition()
{
    Vector3 delta = (m_maximumCoords - m_minimumCoords) / 2.0f;

    OctreeNode<D>* node = new OctreeNode<D>(this, m_depth + 1);
    node->setMinimumCoords(m_minimumCoords);
    node->setMaximumCoords(m_minimumCoords + delta);
    m_childrenNodes.append(node);

    Point3 tmp = m_minimumCoords;
    tmp.x() += delta.x();
    node = new OctreeNode<D>(this, m_depth + 1);
    node->setMinimumCoords(tmp);
    node->setMaximumCoords(tmp + delta);
    m_childrenNodes.append(node);

    tmp = m_minimumCoords;
    tmp.x() += delta.x();
    tmp.y() += delta.y();
    node = new OctreeNode<D>(this, m_depth + 1);
    node->setMinimumCoords(tmp);
    node->setMaximumCoords(tmp + delta);
    m_childrenNodes.append(node);

    tmp = m_minimumCoords;
    tmp.y() += delta.y();
    node = new OctreeNode<D>(this, m_depth + 1);
    node->setMinimumCoords(tmp);
    node->setMaximumCoords(tmp + delta);
    m_childrenNodes.append(node);

    tmp = m_minimumCoords;
    tmp.z() += delta.z();
    node = new OctreeNode<D>(this, m_depth + 1);
    node->setMinimumCoords(tmp);
    node->setMaximumCoords(tmp + delta);
    m_childrenNodes.append(node);

    tmp = m_minimumCoords;
    tmp.x() += delta.x();
    tmp.z() += delta.z();
    node = new OctreeNode<D>(this, m_depth + 1);
    node->setMinimumCoords(tmp);
    node->setMaximumCoords(tmp + delta);
    m_childrenNodes.append(node);

    tmp = m_minimumCoords;
    tmp.x() += delta.x();
    tmp.y() += delta.y();
    tmp.z() += delta.z();
    node = new OctreeNode<D>(this, m_depth + 1);
    node->setMinimumCoords(tmp);
    node->setMaximumCoords(tmp + delta);
    m_childrenNodes.append(node);

    tmp = m_minimumCoords;
    tmp.y() += delta.y();
    tmp.z() += delta.z();
    node = new OctreeNode<D>(this, m_depth + 1);
    node->setMinimumCoords(tmp);
    node->setMaximumCoords(tmp + delta);
    m_childrenNodes.append(node);

    OctreeNode* n;
    int dataCount = m_dataIndices.size();
    for (int i = 0; i < dataCount; ++i)
    {
        foreach(n, m_childrenNodes)
        {
            if(n->isInVolume(m_octree->at(m_dataIndices[i])))
            {
                n->m_dataIndices.append(m_dataIndices[i]);
                break;
            }
        }
    }
    m_dataIndices.clear();
    m_hasChildren = true;
}

template<typename D>
bool OctreeNode<D>::isInVolume(const D& d)
{
    Point3 v = d;
    float x = v.x(), y = v.y(), z = v.z();
    if ( x < m_minimumCoords.x() || x >= m_maximumCoords.x()
        || y < m_minimumCoords.y() || y >= m_maximumCoords.y()
        || z < m_minimumCoords.z() || z >= m_maximumCoords.z()
        )
        {
        return false;
    }

    return true;
}

template<typename D>
QString OctreeNode<D>::toString()
{
    QString res;
    QString indent;
    for (int i = 0; i < m_depth; i++)
        indent+= "  ";

    res = indent + "Node{\n";
    res += indent + "m_depth:" + QString::number(m_depth) + "\n";
    res += indent + "m_minCoords:" + ::toString(m_minimumCoords) + "\n";
    res += indent + "m_maxCoords:" + ::toString(m_maximumCoords);
    qDebug("%s", qPrintable(res));
    if (m_hasChildren) {
        OctreeNode* n;
        res = "";
        foreach(n, m_childrenNodes)
        {
            res += n->toString();
        }
    }
    else {
        res = "";
        int i;
        foreach(i, m_dataIndices)
        {
            res = indent + "point[" + QString::number(i)
                  + "]:" + ::toString(m_octree->toQVector()[i]);
            qDebug("%s", qPrintable(res));
        }
    }
    qDebug(qPrintable(indent + "}"));
    //res = res + indent + "}\n";
    res = "";
    return res;
}

#endif

