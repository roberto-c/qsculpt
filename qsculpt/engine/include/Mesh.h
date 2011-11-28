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
#ifndef MESH_H
#define MESH_H

#include <vector>
#include <utility>
#include "ISurface.h"
#include "SceneNode.h"
#include <QHash>
#include "geometry/Aabb.h"

class Vertex;
class Edge;
class SubdivisionScheme;

/**
 * Class that implement subdivision surfaces.
 */
class Mesh : public ISurface
{
    struct Impl;
    QScopedPointer<Impl> _d;
    
public:
    Mesh();
    virtual ~Mesh();
    /******************************************************************************
     * IObject3D interface
     */
    virtual uint iid() const;
    virtual bool hasChanged();
    virtual void setChanged(bool val);
    virtual void setScene(Scene* scene) ;
    virtual Scene* scene() const;
    virtual Point3 position() const;
    virtual void displace(const Point3& delta);
    virtual void position(float *x, float *y, float *z) const;
    virtual void setOrientation(float rotX, float rotY, float rotZ);
    virtual void orientation(float& rotX, float& rotY, float& rotZ);
    virtual void setPosition(float x, float y, float z);
    virtual void setPosition(const Point3& position);
    virtual Eigen::Affine3f transform() const;
    virtual Eigen::Affine3f & transform();
    virtual void setTransform(const Eigen::Affine3f & transform);
    virtual const geometry::AABB& boundingBox() const;
    virtual void setColor(const Color& color);
    virtual Color color() const;
    virtual void setSelected(bool val);
    virtual bool isSelected() const;
    virtual int addVertex(const Point3& point);
    virtual int addVertex(Vertex* v);
    virtual void removeVertex(int id);
    virtual Vertex* vertex(int index);
    virtual const Vertex* vertex(int index) const;
    virtual int numVertices() const;
    virtual int addEdge(const Edge& edge);
    virtual int addEdge(int v1, int v2);
    virtual int addFace(const QVector<int>& vertexIndexList);
    virtual void replaceFace(int index, const QVector<int>& vertexIndexList);
    virtual void removeFace( int id);
    virtual int numFaces() const;
    virtual Face* face(int index);
    virtual int getFaceIndexAtPoint(const Point3& p) const;
    virtual int getClosestPointAtPoint(const Point3 &p) const;
    virtual QVector<int> getPointsInRadius(const Point3 &p, float radius) const;
    virtual void adjustPointNormal(int index);
    virtual void lock() const;
    virtual void unlock() const;
    
    virtual QVector<int> selectedPoints() const;
    virtual void setSelectedPoints(const QVector<int>& indicesArray);
    
    virtual Iterator<Vertex> vertexIterator();
    virtual Iterator<Vertex> constVertexIterator() const;
    virtual Iterator<Face> faceIterator();
    virtual Iterator<Face> constFaceIterator() const;
    virtual Iterator<Edge> edgeIterator();
    virtual Iterator<Edge> constEdgeIterator() const;
    
    Point3 localToWorldCoords(const Point3& p) const;
    
    Point3 worldToLocalCoords(const Point3& p) const;
    
    // End IObject3D interface
    
    //const Object3D& operator=(const Object3D& obj);
    
    /**
     * Debugging function used to print the memory usage of this
     * object.
     */
    void printMemoryInfo() const;
    
protected:
    /**
     * Initializes the points vector.
     */
    virtual void initPoints();
    
    /**
     *
     */
    int addEdge(Vertex* v1, Vertex* v2);
    
    
    // inner classes
protected:
    class VertexIterator;
    class FaceIterator;
    class EdgeIterator;
    
    friend class VertexIterator;
    friend class FaceIterator;
    friend class EdgeIterator;
};
#endif

