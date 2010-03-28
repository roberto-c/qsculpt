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
#ifndef OBJECT3D_H
#define OBJECT3D_H

#include <QColor>
#include <QMutex>
#include <QHash>
#include "IObject3D.h"

class Scene;
struct Face;
struct Edge;
//struct Point;

/**
 * Base class for all 3D objects.
 *
 *  @author Juan Roberto Cabral Flores <roberto.cabral@gmail.com>
 */
class Object3D : public IObject3D
{
public:
    Object3D();

    //Object3D(const Object3D& cpy);

    virtual ~Object3D();

/******************************************************************************
 * IObject3D interface
 */
    virtual void setScene(Scene* scene) ;
    virtual Scene* getScene() const;
    virtual Point3 getPosition() const;
    virtual void displace(const Point3& delta);
    virtual void getPosition(float *x, float *y, float *z) const;
    virtual void rotate(float rotX, float rotY, float rotZ);
    virtual void setPosition(float x, float y, float z);
    virtual void setPosition(const Point3& position);
    virtual float getDepth();
    virtual float getHeight();
    virtual float getWidth();
    virtual void setDepth(float value);
    virtual void setHeight(float value);
    virtual void setWidth(float value);
    virtual void setColor(const QColor& color);
    virtual QColor getColor() const;
    virtual void setSelected(bool val);
    virtual bool isSelected() const;
    virtual void setBoundingBoxColor(const QColor& color);
    virtual QColor getBoundingBoxColor() const;
    virtual int addVertex(const Point3& point);
    virtual void removeVertex(int id);
    virtual Point3& getVertex(int index);
	virtual Point3 getVertex(int index) const;
	virtual int getNumVertices() const;
    virtual Vector3& getNormalAtPoint(int index);
    virtual const Vector3& getNormalAtPoint(int index) const;
	virtual int addEdge(const Edge& edge);
	virtual int addEdge(int v1, int v2);
    virtual int addFace(const QVector<int>& vertexIndexList);
	virtual void replaceFace(int index, const QVector<int>& vertexIndexList);
    virtual void removeFace( int id);
	virtual int getNumFaces() const;
    virtual int getFaceIndexAtPoint(const Point3& p) const;
    virtual int getClosestPointAtPoint(const Point3 &p) const;
    virtual QVector<int> getPointsInRadius(const Point3 &p, float radius) const;
    virtual void adjustPointNormal(int index);
    virtual const NormalContainer& getNormalList() const;
    virtual const PointContainer& getPointList() const;
    virtual const FaceContainer& getFaceList() const;
    virtual NormalContainer& getNormalList();
    virtual PointContainer& getPointList();
    virtual FaceContainer& getFaceList();
	virtual const EdgeContainer& getEdgeList() const;
	virtual EdgeContainer& getEdgeList();
    virtual void lock() const;
    virtual void unlock() const;
    virtual void addResolutionLevel();
    virtual void removeResolutionLevel(int level);
    virtual void setWorkingResolutionLevel(int level);
    virtual int getWorkingResolutionLevel();
	virtual bool hasChanged() {
		return m_hasChanged;
	};
	virtual void setChanged(bool val) {
		m_hasChanged = val;
		if (val) {
			emit meshChanged(this);
		}
	};
	virtual QVector<int> getSelectedPoints() const {
		return m_selectedPoints;
	}
	virtual void setSelectedPoints(const QVector<int>& indicesArray) {
		m_selectedPoints = indicesArray;
	}
	
	virtual Iterator<Vertex> vertexIterator();
	virtual Iterator<Vertex> constVertexIterator() const;
	virtual Iterator<Face> faceIterator();
	virtual Iterator<Face> constFaceIterator() const;
	
// End IObject3D interface

    //const Object3D& operator=(const Object3D& obj);

protected:
    /**
     * Initializes the points vector.
     */
    virtual void initPoints();

    /**
     * Draw a bounding box around the object.
     */
    void drawBoundingBox();

    /**
     * Scales the object to a factor. This method loops by each point and
     * applies the corresponding factor of each component.
     *
     * @param xfactor factor to apply on the x compoent of each point.
     * @param yfactor factor to apply on the y compoent of each point.
     * @param zfactor factor to apply on the z compoent of each point.
     */
    void scale(float xfactor, float yfactor, float zfactor);

    /**
     *
     */
    Vector3 computeFaceNormal(int index);

    /**
     *
     */
    Vector3 computeFaceNormal(Face &face);

    /**
     *
     */
    void computeAllNormals();

    Scene*          m_scene;
    Point3          m_position;
    QColor          m_color,
                    m_boundingBoxColor;
    Point3          m_boundingBoxVert[8];
    float           m_minX,
                    m_maxX,
                    m_minY,
                    m_maxY,
                    m_minZ,
                    m_maxZ;
    float           m_rotX, m_rotY, m_rotZ;
    bool            m_selected;
    int             m_callListId;
    bool            m_genereateCallList;
    int				m_currentResolutionLevel;
	bool			m_hasChanged;
	QVector<int>	m_selectedPoints;

private:
    void updateBoundingBox();
	
	/* Subdivide the object from one level and set faces that result in
	 * a new level of resolution.
	 */
	bool subdivide(int fromLevel);

    PointContainer  			m_pointList;
    NormalContainer 			m_normalList;
    QVector<FaceContainer*>   	m_faceList;
	QVector<EdgeContainer*>		m_edgeList;
    mutable QMutex    			m_mutex;
	QVector<Edge*>				m_hedgeList;
	
// inner classes
public:
	class VertexIterator : public IIterator<Vertex>
	{	
		friend class Object3D;
		
		const Object3D* _v;
		Edge* _currHe;
		
	protected:
		/**
		 * Constructor of a vertex iterator. The vertex iterator
		 * is only contructed by means of Vertex::vertexIterator() function
		 */
		VertexIterator(const Object3D* v);
		
	public:
		/**
		 * Return true if the iterator has more elements (i.e. it is not at the
		 * end)
		 */
		bool hasNext() const;
		
		/**
		 * Returns true if the iterator is not at the beginning of the iteration
		 */
		bool hasPrevious() const;
		
		/**
		 * Returns the next element and advance the iterator by one.
		 */
		Vertex & next();
		
		/**
		 * Returns the next element and advance the iterator by one.
		 */
		const Vertex & next() const;
		
		/**
		 * Returns the previous elements and move the iterator one position
		 * backwards.
		 */
		Vertex & previous();
		
		/**
		 * Returns the previous elements and move the iterator one position
		 * backwards.
		 */
		const Vertex & previous() const;
	};
	
	class FaceIterator : public IIterator<Face>
	{
		friend class Object3D;
		
		const Object3D* _v;
		Edge* _currHe;
		
	protected:
		/**
		 * Constructor of a vertex iterator. The vertex iterator
		 * is only contructed by means of Vertex::vertexIterator() function
		 */
		FaceIterator(const Object3D* v);
		
	public:
		/**
		 * Return true if the iterator has more elements (i.e. it is not at the
		 * end)
		 */
		bool hasNext() const;
		
		/**
		 * Returns true if the iterator is not at the beginning of the iteration
		 */
		bool hasPrevious() const;
		
		/**
		 * Returns the next element and advance the iterator by one.
		 */
		Face & next();
		
		/**
		 * Returns the next element and advance the iterator by one.
		 */
		const Face & next() const;
		
		/**
		 * Returns the previous elements and move the iterator one position
		 * backwards.
		 */
		Face & previous();
		
		/**
		 * Returns the previous elements and move the iterator one position
		 * backwards.
		 */
		const Face & previous() const;
	};
	
	friend class VertexIterator;
	friend class FaceIterator;
};

#endif

