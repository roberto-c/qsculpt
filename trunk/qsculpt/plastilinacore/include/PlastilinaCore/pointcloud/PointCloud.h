//
//  PointCloud.h
//  PlastilinaCore
//
//  Created by Juan Roberto Cabral Flores on 8/5/12.
//
//

#ifndef PlastilinaCore_PointCloud_h
#define PlastilinaCore_PointCloud_h

#include <PlastilinaCore/ISurface.h>

class PointCloud : public ISurface
{
    
public:
    typedef std::shared_ptr<PointCloud>   shared_ptr;
    typedef std::weak_ptr<PointCloud>     weak_ptr;
    typedef std::unique_ptr<PointCloud>   Ptr;
    
public:
    /**
     * Default contructor. The classes that implement this interface, should
     * have a default contructor that initiliazes the object with valid values.
     */
    PointCloud(){}
    
    virtual PointCloud(){}
    
    /**
     * Instance id of the surface
     */
    virtual size_t iid() const ;
    
    
    virtual void setScene(Scene* scene) ;
    
    virtual Scene* scene() const ;
    
    /**
     * Returns the bounding box of the object.
     *
     */
    virtual const geometry::AABB& boundingBox() const ;
    
    /**
     * Sets the object base color. Used as vertex color, line color, shading
     * color, etc.
     *
     * @param color color of the object.
     *
     */
    virtual void setColor(const Color& color) ;
    
    /**
     * Gets the object base color. Used as vertex color, line color, shading
     * color, etc.
     *
     * @return color of the object.
     *
     */
    virtual Color color() const ;
    
    /**
     * Mark the object as selected. This property is used by the renderers
     * to mark selected objects (for example draw a bounding box around the
     * object)
     *
     * @param val pass true to set the object as selected
     */
    virtual void setSelected(bool val) ;
    
    /**
     * Returns the selected state of the object.
     *
     * @return true if the object is selected, otherwise, false.
     * @see setSelected
     */
    virtual bool isSelected() const ;
    
    /**
     * Add point to object. The point added is in local coordinates ( i.e.
     * relative to object's origin.
     *
     * @param point point to add to the object. The point is in local coords.
     *
     * @return ID of the new point.
     */
    virtual size_t addVertex(const Point3& point) ;
    
    /**
     * Add vertex v to this object. The object will mantain the reference
     * to the vertex. Do not delete the vertex while the object has a reference
     * to it.
     *
     * @param v vertex to add to the object.
     *
     * @return the id of the vertex inside the object.
     */
    virtual size_t addVertex(Vertex* v) ;
    
    /**
     * Remove a point from the object. If the point form part of a
     * triangle then that triangle should be removed from the object.
     *
     * @param id ID of the point to delete.
     */
    virtual void removeVertex( size_t id) ;
    
    /**
     * Returns the vertex with the instance ID iid.
     */
    virtual Vertex* vertex(size_t iid) ;
    
    /**
     * Returns the vertex with the instance ID iid.
     */
    virtual const Vertex* vertex(size_t iid) const ;
    
    /**
     * Gets the number of vertices
     */
    virtual size_t numVertices() const ;
    
    /**
     *
     */
    virtual size_t addEdge(const Edge& edge) ;
    
    /**
     *
     */
    virtual size_t addEdge(size_t v1, size_t v2) ;
    
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
    virtual size_t addFace( const std::vector<size_t>& vertexIndexes );
    
    /**
     * Replace a triangle with new indices to existing points.
     */
    virtual void replaceFace(size_t index, const std::vector<size_t>& vertexIndexList);
    
    /**
     * Remove the triangle from the object. The points or vertices that
     * compound the triangle are not removed from the object. The points
     * could be rendered acording to the drawing mode.
     *
     * @param id triangle ID to remove.
     */
    virtual void removeFace( size_t id) ;
    
    /**
     * Returns the number of faces that the object has.
     *
     * TODO: This may not belong here as is specific to mesh surfaces.
     */
    virtual size_t numFaces() const ;
    
    /**
     * Returns the face at the position index.
     */
    virtual Face* face(size_t index) ;
    
    /**
     *
     */
    virtual size_t getFaceIndexAtPoint(const Point3& p) const ;
    
    /**
     *
     */
    virtual size_t getClosestPointAtPoint(const Point3 &p) const ;
    
    /**
     *
     */
    virtual std::vector<size_t> getPointsInRadius(const Point3 &p, float radius) const ;
    
    /**
     *
     */
    virtual void adjustPointNormal(size_t index) ;
    
    /**
     *
     */
    virtual void lock() const ;
    
    /**
     *
     */
    virtual void unlock() const ;
    
    /**
     * Flag to check if the surface has been modified.
     */
    virtual bool hasChanged();
    
    /**
     * Sets a flag to mark the surface as modified.
     */
    virtual void setChanged(bool val);
    
    /**
     *
     */
    virtual std::vector<size_t> selectedPoints() const ;
    
    /*
     *
     */
    virtual void setSelectedPoints(const std::vector<size_t>& indicesArray) ;
    
    /**
     * Returns a vertex iterator to traverse over all the vertices in this
     * object.
     */
    virtual Iterator<Vertex> vertexIterator() ;
    virtual Iterator<Vertex> constVertexIterator() const ;
    
    /**
     * Returns a face iterator to traverse over all the faces of this object.
     */
    virtual Iterator<Face> faceIterator() ;
    
    /**
     * Return a const iterator over the faces that contained in this object.
     */
    virtual Iterator<Face> constFaceIterator() const ;
    
    /**
     *
     */
    virtual Iterator<Edge> edgeIterator() ;
    
    /**
     *
     */
    virtual Iterator<Edge> constEdgeIterator() const ;
    
    /**
     * Used to notify mesh has changed.
     */
    void meshChanged(ISurface* mesh) {
        
    }
};

#endif
