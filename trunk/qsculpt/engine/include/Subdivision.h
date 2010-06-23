/*
 *  Subdivision.h
 *  QSculpt
 *
 *  Created by Roberto Cabral on 12/18/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef SUBDIVISION_H
#define SUBDIVISION_H

#include <vector>
//#include <hash_map>
#include "ISurface.h"
#include "SceneNode.h"

class Vertex;
class Edge;
class SubdivisionScheme;

/**
 * Class that implement subdivision surfaces.
 */
class Subdivision : public ISurface, public SceneNode
{
	typedef std::vector<Vertex*> VertexCollection;
	typedef std::vector<Edge*> EdgesCollection;
	typedef std::vector<Face*> FacesCollection;
	
	/** Vertices by each subdivision level */
	std::vector<VertexCollection> _vertLevelCollections;
	/** Edges by each subdivision level */
	std::vector<EdgesCollection> _edgesLevelCollections;
	std::vector<FacesCollection> _facesLevelCollections;
	
	VertexCollection *_vertices;
	EdgesCollection *_edges;
	FacesCollection *_faces;
	
	SubdivisionScheme* scheme;
	
public:
	Subdivision();
	virtual ~Subdivision();
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
    virtual int addVertex(Vertex* v);
    virtual void removeVertex(int id);
    virtual Point3& getVertex(int index);
	virtual Point3 getVertex(int index) const;
	virtual int getNumVertices() const;
	virtual int addEdge(const Edge& edge);
	virtual int addEdge(int v1, int v2);
    virtual int addFace(const QVector<int>& vertexIndexList);
	virtual void replaceFace(int index, const QVector<int>& vertexIndexList);
    virtual void removeFace( int id);
	virtual int getNumFaces() const;
    virtual Face& getFace(int index);
    virtual int getFaceIndexAtPoint(const Point3& p) const;
    virtual int getClosestPointAtPoint(const Point3 &p) const;
    virtual QVector<int> getPointsInRadius(const Point3 &p, float radius) const;
    virtual void adjustPointNormal(int index);
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
	
    Iterator<Vertex> vertexIterator(int level);
	Iterator<Vertex> constVertexIterator(int level) const;
	Iterator<Face> faceIterator(int level);
	Iterator<Face> constFaceIterator(int level) const;
    
protected:
    /**
     * Initializes the points vector.
     */
    virtual void initPoints();
	
    /**
     * Draw a bounding box around the object.
     */
    void drawBoundingBox();
	
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

	
	// inner classes
protected:
    class VertexIterator;
    class FaceIterator;
	
	friend class VertexIterator;
	friend class FaceIterator;
};

#endif
