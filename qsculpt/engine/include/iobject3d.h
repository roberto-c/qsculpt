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
#ifndef IOBJECT3D_H
#define IOBJECT3D_H

#include <QVector>
#include "point3d.h"
#include "spenums.h"
#include "octree.h"

/*
 * Class forward declaration
 */
class QColor;
class Scene;
struct Face;
struct Edge;
struct Point;

/**
Interface that every 3D object should implement.
 
  @author Juan Roberto Cabral Flores <roberto.cabral@gmail.com>
*/
class IObject3D {
public:
    /**
     * Default contructor. The classes that implement this interface, should
     * have a default contructor that initiliazes the object with valid values.
     */
    IObject3D(){}

    virtual ~IObject3D(){}

    virtual void setScene(Scene* scene) = 0;
    
    virtual Scene* getScene() const = 0;
    
    /**
     * Returns the capabilitites of the object. This method can be used to
     * know if the object can/can't support some operations. 
     */
    //virtual Capabilities getCapabilities() =0;

    /**
     * Set the object's position.
     *
     * @param x Object's x position
     * @param y Object's y postion
     * @param z Object's z position
     */
    virtual void setPosition (float x, float y, float z) = 0;

    /**
     * Set the object's position. Overloaded method.
     *
     * @param position Object position.
     */
    virtual void setPosition (Point3D position) = 0;

    /**
     * Get the object's position.
     * 
     * @param x Object x-axis position.
     * @param y Object y-axis position.
     * @param z Object z-axis position.
     */
    virtual void getPosition (float *x, float *y, float *z) const = 0;

    /**
     * Get the object's position. Overloaded method.
     *
     * @return position of the object.
     */
    virtual Point3D getPosition () const = 0;

    /**
     * Move the object.
     * Move the object relative to the actual position.
     *
     * new_position = current_position + delta.
     *
     * @param delta Point that contains the amount of displacement for each axis.
     */
    virtual void displace (Point3D delta) = 0;

    /**
     * Rotate the object. This rotation is applied over the local object axis.
     * 
     * @param rotX Rotation angle in x-axis.
     * @param rotY Rotation angle in y-axis.
     * @param rotZ Rotation angle in z-axis.
     */
    virtual void rotate (float rotX, float rotY, float rotZ) = 0;

    /**
     * Draws the object on the screen.
     * 
     */
    virtual void draw() = 0;

    /**
     * Returns the depth of the object.
     *
     */
    virtual float getDepth() = 0;

    /**
     * Returns the height of the object.
     *
     */
    virtual float getHeight() = 0;

    /**
     * Returns the width of the object.
     *
     */
    virtual float getWidth() = 0;

    /**
     * Sets the depth of the object. 
     *
     * @param value new depth value
     */
    virtual void setDepth(float value) = 0;

    /**
     * Sets the height of the object.
     *
     * @param value new height value
     */
    virtual void setHeight(float value) = 0;

    /**
     * Sets the width of the object. 
     *
     * @param value new width value
     */
    virtual void setWidth(float value) = 0;
    
    /**
    * Sets the drawing mode for the object
    *
    * @param mode one of the DrawingMode enum values.
    */
    virtual void setDrawingMode(DrawingMode mode) = 0;

    /**
    * Sets the object base color. Used as vertex color, line color, shading
    * color, etc.
    *
    * @param color color of the object.
    *
    */
    virtual void setColor(QColor color) = 0;

    /**
    * Gets the object base color. Used as vertex color, line color, shading
    * color, etc.
    *
    * @return color of the object.
    *
    */
    virtual const QColor getColor() = 0;
    
    /**
     * Draw a bounding box around the object.
     *
     * @param val pass true to draw a bounding box around the
     * object.
     */
    virtual void showBoundingBox(bool val) = 0;
    
    /**
     * Draw a bounding box around the object.
     *
     * @return true if the object has a bounding box, otherwise, false.
     */
    virtual bool getShowBoundingBox() = 0;
    
    /**
     * Set the color of the bounding box.
     *
     * @param color color of bounding box.
     */
    virtual void setBoundingBoxColor(QColor color) = 0;
    
    /**
     * Get the color of the bounding box.
     *
     * @return color of bounding box.
     */
    virtual QColor getBoundingBoxColor() = 0;
    
    /**
     * Add point to object. The point added is in local coordinates ( i.e. 
     * relative to object's origin.
     *
     * @param point point to add to the object. The point is in local coords.
     * 
     * @return ID of the new point.
     */
    virtual int addPoint(Point3D point) = 0;
    
    /**
     * Remove a point from the object. If the point form part of a
     * triangle then that triangle should be removed from the object.
     *
     * @param id ID of the point to delete.
     */
    virtual void removePoint( int id) = 0;
    
    /**
     * 
     */
    virtual Point3D& getPoint(int index) = 0;
    
    virtual Normal& getNormalAtPoint(int index) = 0;
    
    virtual const Normal& getNormalAtPoint(int index) const = 0;
    
    /**
     * Add a triangle to the object. The triangle is formed by the vertices
     * passed as arguments. The vertices are passed by means of their ID
     * 
     * @param v1 id of the first vertex that form the triangle.
     * @param v2 id of the first vertex that form the triangle.
     * @param v3 id of the first vertex that form the triangle.
     *
     * @return triangle ID
     */
    virtual int addFace( const QVector<int>& vertexIndexes )=0;
    
    /**
     * Remove the triangle from the object. The points or vertices that
     * compound the triangle are not removed from the object. The points
     * could be rendered acording to the drawing mode.
     *
     * @param id triangle ID to remove.
     */
    virtual void removeFace( int id) =0;
    
	/**
	 *
	 */
	virtual int getFaceIndexAtPoint(const Point3D& p) const = 0;
    
    /**
     * 
     */
    virtual int getClosestPointAtPoint(const Point3D &p) const = 0;
    
    /**
     * 
     */
    virtual QVector<int> getPointsInRadius(const Point3D &p, float radius) const = 0;
    
    /**
     * 
     */
    //virtual void markTriangle(int index, bool state) = 0;
    
    /**
     * Adds more triangles to the model by face sub-division.
     */
    //virtual void subdivide() = 0;
    
    /**
     * 
     */
    virtual void adjustPointNormal(int index) = 0;
    
	/**
	 *
	 */
    virtual const QVector<Normal>& getNormalList() const = 0;
    
	/**
	 *
	 */
    virtual const Octree<Point>& getPointList() const = 0;
    
	/**
	 *
	 */
    virtual const QVector<Face>& getFaceList() const = 0;
	
	/**
	 *
	 */
	virtual QVector<Normal>& getNormalList() = 0;
    
	/**
	 *
	 */
    virtual Octree<Point>& getPointList()  = 0;
    
	/**
	 *
	 */
    virtual QVector<Face>& getFaceList()  = 0;
	
	/**
	 *
	 */
	virtual void lock() = 0;
	
	/**
	 *
	 */
	virtual void unlock() = 0;
};

struct Point
{
	Vertex vertex;
	QVector<int> faceRef;
	
	Point() {}
	Point(float x, float y, float z) : vertex(x, y, z){}
	Point(const Vertex& v) : vertex(v) {}
	
	operator Vertex() {
		return vertex;
	}
	
	operator Vertex() const {
		return vertex;
	}
	
	bool operator==(const Vertex& v) const {
		return vertex == v;
	}
	
	bool operator==(const Point& p) const {
		return vertex == p.vertex;
	}
};

/**
* Used for the subdivision process. Used to store the different lines used
 * by each triangle to make sure that not duplicate points are contructed
 * when doing the process.
 */
struct Edge
{
    int point1;
    int point2;
    int midPoint;
	
    QVector<int> faceRef;
    
    /**
		* Default constructor, initializes the values on 0
     */
    Edge()
        : point1(-1),
        point2(-1),
        midPoint(-1)
    {
    }
	
    /**
		* Initializes the values on according to the parameters passed.
     * 
     * @param p1 index of the first point.
     * @param p2 index of the second point.
     */
    Edge(int p1, int p2)
        :   point1(p1),
		point2(p2),
		midPoint(-1)
    {
    }
    
    bool operator==(const Edge& val)
    {
        return (point1 == val.point1 && point2 == val.point2)
		|| (point2 == val.point1 && point1 == val.point2);
    }
};

/**
* Face class. This class conatains references to points that should
 * form a triangle.
 *
 * The points are indices of the Object3D point list. So, this class It's
 * only meant to be used with the Object3D class (its not a generic triangle
												  * class).
 *
 * A triangle its only valid if theirs three points are different. Using the
 * default constructor makes a non valid triangle.
 * @author Juan Roberto Cabral Flores <roberto.cabral@gmail.com>
 */
struct Face
{
    QVector<int> point;   /**< Index of first point */
    QVector<int> normal;  /**< Index of normal */
    bool isMarked;
    
    /**
		* Default constructor. Initiliazes all point to index 0.
     */
    Face()
        :   point(),
		normal(),
		isMarked(false)
	{
	}
	
    /**
		* Construct a new triangle. The triangle is composed by the passed
     * points ids or references.
     *
     * @param p1 first point that form the triangle.
     * @param p2 second point that form the triangle.
     * @param p3 third point that form the triangle.
     */
    Face(const QVector<int>& vertexIndexList)
        :   point(vertexIndexList),
		isMarked(false)
    {
			normal.fill(-1, point.size());
    }
	
    /**
		* Checks if the triangle data is valid. Triangle is valid only if
     * the three point that compound it are different.
     *
     * @return true if triangle is valid. False, otherwise.
     */
    bool isValid() { 
        return !point.isEmpty();
    }
	
    /**
		* Sets the first point index reference.
     *
     * @param p1 index of the first point.
     * @param p2 index of the second point.
     * @param p3 index of the third point.
     */
    void setPoints(const QVector<int>& vertexIndexList) { 
        point = vertexIndexList;
        normal.resize(point.size());
    }
	
    bool hasEdge(const Edge& e) const {
        return hasEdge(e.point1, e.point2);
    }
	
    bool hasEdge(int v1, int v2) const {
        bool res = false;
        if (int index = point.indexOf(v1) != -1)
        {
            if (index == point.size() - 1)
                res = v2 == point[0];
            else
                res = v2 == point[index + 1];
        }
        return res;
    }
    
    bool operator==(const Face& t) const {
        int psize = point.size();
        if (psize == 0 || psize != t.point.size())
            return false;
        
        int elementIndex = t.point.indexOf(point[0]);
        if (elementIndex == -1 )
            return false;
        
        for (int i = 1; i < psize; ++i)
        {
            elementIndex = (elementIndex + 1 ) % psize;
            if (point.at(i) != t.point.at(elementIndex) )
                return false;
        }
        return true;
    }
};


#endif
