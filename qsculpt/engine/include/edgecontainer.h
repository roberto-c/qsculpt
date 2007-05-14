/*
 *  edgecontainer.h
 *  qsculpt
 *
 *  Created by Roberto Cabral on 5/12/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef EDGECONTAINER_H_
#define EDGECONTAINER_H_

#include <QVector>
#include <QMultiHash>

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
    
    QList<int> faceRef;
    
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
    
    bool operator==(const Edge& val) const
    {
        return (point1 == val.point1 && point2 == val.point2)
        || (point2 == val.point1 && point1 == val.point2);
    }
};

class EdgeContainer
{
public:
	void reserve(int size) {
		m_edgeList.reserve(size);
		m_edgeList.reserve(size);
	}
	
	int size() const {
		return m_edgeList.size();
	}
	
	bool contains(const Edge& edge) {
		return m_edgeHash.contains(edge);
	}
	
	Edge at(int index) {
		return m_edgeList.at(index);
	}
	
	int insert(const Edge& edge) {
		m_edgeList.append(edge);
		int index =  m_edgeList.size() -1;
		m_edgeHash.insert(edge,index);
		return index;
	}
	
	void append(const Edge& edge) {
		m_edgeList.append(edge);
		m_edgeHash.insert(edge, m_edgeList.size() -1);
	}
	
	int indexOf(const Edge& edge) {
		if (!m_edgeHash.contains(edge))
			return -1;
		return m_edgeHash.value(edge);
	}
	
	void addFaceReference(int index, int faceIndex) {
		m_edgeList[index].faceRef.append(faceIndex);
	}
	
	void setMidPointReference(int index, int midPointIndex) {
		m_edgeList[index].midPoint = midPointIndex;
	}
	
private:
	EdgeContainer(){}
	
	// undefined: class does not make copies of it self
	EdgeContainer(const EdgeContainer&);
	EdgeContainer operator=(const EdgeContainer&);
	
	QVector<Edge> m_edgeList;
	QHash<Edge, int> m_edgeHash;
	
	friend class Object3D;
};

#endif
