//
//  PointCloud.cpp
//  PlastilinaCore
//
//  Created by Juan Roberto Cabral Flores on 8/5/12.
//
//

#include <PlastilinaCore/pointcloud/PointCloud.h>

/**
 * Default contructor. The classes that implement this interface, should
 * have a default contructor that initiliazes the object with valid values.
 */
PointCloud::PointCloud(){}

PointCloud::PointCloud(){}

/**
 * Instance id of the surface
 */
size_t PointCloud::iid() const
{
    
}


void PointCloud::setScene(Scene* scene)
{
    
}

Scene* PointCloud::scene() const
{
    
}

/**
 * Returns the bounding box of the object.
 *
 */
const geometry::AABB& boundingBox() const
{
    
}

/**
 * Sets the object base color. Used as vertex color, line color, shading
 * color, etc.
 *
 * @param color color of the object.
 *
 */
void PointCloud::setColor(const Color& color)
{
    
}

/**
 * Gets the object base color. Used as vertex color, line color, shading
 * color, etc.
 *
 * @return color of the object.
 *
 */
Color PointCloud::color() const ;

/**
 * Mark the object as selected. This property is used by the renderers
 * to mark selected objects (for example draw a bounding box around the
 * object)
 *
 * @param val pass true to set the object as selected
 */
void PointCloud::setSelected(bool val) ;

/**
 * Returns the selected state of the object.
 *
 * @return true if the object is selected, otherwise, false.
 * @see setSelected
 */
bool PointCloud::isSelected() const ;

/**
 * Add point to object. The point added is in local coordinates ( i.e.
 * relative to object's origin.
 *
 * @param point point to add to the object. The point is in local coords.
 *
 * @return ID of the new point.
 */
size_t PointCloud::addVertex(const Point3& point) ;

/**
 * Add vertex v to this object. The object will mantain the reference
 * to the vertex. Do not delete the vertex while the object has a reference
 * to it.
 *
 * @param v vertex to add to the object.
 *
 * @return the id of the vertex inside the object.
 */
size_t PointCloud::addVertex(Vertex* v) ;

/**
 * Remove a point from the object. If the point form part of a
 * triangle then that triangle should be removed from the object.
 *
 * @param id ID of the point to delete.
 */
void PointCloud::removeVertex( size_t id) ;

/**
 * Returns the vertex with the instance ID iid.
 */
Vertex* PointCloud::vertex(size_t iid) ;

/**
 * Returns the vertex with the instance ID iid.
 */
const Vertex* PointCloud::vertex(size_t iid) const ;

/**
 * Gets the number of vertices
 */
size_t PointCloud::numVertices() const ;

/**
 *
 */
size_t PointCloud::addEdge(const Edge& edge) ;

/**
 *
 */
size_t PointCloud::addEdge(size_t v1, size_t v2) ;

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
size_t PointCloud::addFace( const std::vector<size_t>& vertexIndexes );

/**
 * Replace a triangle with new indices to existing points.
 */
void PointCloud::replaceFace(size_t index, const std::vector<size_t>& vertexIndexList);

/**
 * Remove the triangle from the object. The points or vertices that
 * compound the triangle are not removed from the object. The points
 * could be rendered acording to the drawing mode.
 *
 * @param id triangle ID to remove.
 */
void PointCloud::removeFace( size_t id) ;

/**
 * Returns the number of faces that the object has.
 *
 * TODO: This may not belong here as is specific to mesh surfaces.
 */
size_t PointCloud::numFaces() const ;

/**
 * Returns the face at the position index.
 */
Face* PointCloud::face(size_t index) ;

/**
 *
 */
size_t PointCloud::getFaceIndexAtPoint(const Point3& p) const ;

/**
 *
 */
size_t PointCloud::getClosestPointAtPoint(const Point3 &p) const ;

/**
 *
 */
std::vector<size_t> getPointsInRadius(const Point3 &p, float radius) const ;

/**
 *
 */
void adjustPointNormal(size_t index) ;

/**
 *
 */
void lock() const ;

/**
 *
 */
void unlock() const ;

/**
 * Flag to check if the surface has been modified.
 */
bool hasChanged();

/**
 * Sets a flag to mark the surface as modified.
 */
void setChanged(bool val);

/**
 *
 */
std::vector<size_t> selectedPoints() const ;

/*
 *
 */
void setSelectedPoints(const std::vector<size_t>& indicesArray) ;

/**
 * Returns a vertex iterator to traverse over all the vertices in this
 * object.
 */
Iterator<Vertex> vertexIterator() ;
Iterator<Vertex> constVertexIterator() const ;

/**
 * Returns a face iterator to traverse over all the faces of this object.
 */
Iterator<Face> faceIterator() ;

/**
 * Return a const iterator over the faces that contained in this object.
 */
Iterator<Face> constFaceIterator() const ;

/**
 *
 */
Iterator<Edge> edgeIterator() ;

/**
 *
 */
Iterator<Edge> constEdgeIterator() const ;

/**
 * Used to notify mesh has changed.
 */
void meshChanged(ISurface* mesh) {
    
}