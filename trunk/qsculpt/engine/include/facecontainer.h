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

#ifndef FACECONTAINER_H
#define FACECONTAINER_H

/**
* Face class. This class contains references to points that should
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
	QVector<int> edge;
	int midPoint;
    bool isMarked;
	qint32 hashValue;
    
    /**
		* Default constructor. Initiliazes all point to index 0.
     */
    Face()
        :   point(),
        normal(),
		midPoint(-1),
        isMarked(false),
		hashValue(0)
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
		midPoint(-1),
        isMarked(false),
		hashValue(0)
    {
		normal.fill(-1, point.size());
		for(int i = 0; i < point.size(); ++i)
			hashValue += point[i];
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
		hashValue = 0;
		for(int i = 0; i < point.size(); ++i)
			hashValue += point[i];
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
	
	operator Vertex()
	{
		Vertex vtx;
		return vtx;
	}
	
	operator Vertex() const
	{
		Vertex vtx;
		return vtx;
	}
};

class FaceContainer
{
public:
	void reserve(int size) {
		m_faceList.reserve(size);
		m_faceHash.reserve(size);
	}
	
	int size() const {
		return m_faceList.size();
	}
	
	bool contains(const Face& face) {
		return m_faceHash.contains(face);
	}
	
	Face& operator[](int index) {
		return m_faceList[index];
	}
	
	const Face& operator[](int index) const {
		return m_faceList[index];
	}
	
	const Face& at(int index) const {
		return m_faceList.at(index);
	}
	
	int insert(const Face& face) {
		m_faceList.append(face);
		int index =  m_faceList.size() -1;
		m_faceHash.insert(face,index);
		return index;
	}
	
	void append(const Face& face) {
		m_faceList.append(face);
		m_faceHash.insert(face, m_faceList.size() -1);
	}
	
	void remove(int index) {
		m_faceHash.remove(m_faceList[index]);
		m_faceList.remove(index);
	}
	
	int indexOf(const Face& face) {
		if (!m_faceHash.contains(face))
			return -1;
		return m_faceHash.value(face);
	}
		
	void setMidPointReference(int index, int midPointIndex) {
		m_faceList[index].midPoint = midPointIndex;
	}
	
	void addEdgeReference(int index, int edgeIndex) {
		if (m_faceList[index].edge.indexOf(edgeIndex) == -1)
			m_faceList[index].edge.append(edgeIndex);
	}
	
private:
	FaceContainer(){}
	
	// undefined: class does not make copies of itself
	FaceContainer(const FaceContainer&);
	FaceContainer operator=(const FaceContainer&);
	
	Octree<Face> m_faceList;
	QHash<Face, int> m_faceHash;
	
	friend class Object3D;
};

inline uint qHash(const Face& key)
{
	return qHash( key.hashValue );
}

#endif
