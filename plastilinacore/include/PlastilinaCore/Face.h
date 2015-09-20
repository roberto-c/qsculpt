/*
 *  untitled.h
 *  QSculpt
 *
 *  Created by Roberto Cabral on 12/4/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef FACE_H_
#define FACE_H_

#include <PlastilinaCore/Vertex.h>
#include <PlastilinaCore/IIterator.h>

class EdgeHandle;
class ISurface;

enum FaceFlags {
    FF_None     = 0,
    FF_Selected = 0x00000001,   /*< Face is selected */
    FF_Deleted  = 0x00000002,  /*< Face is marked as deleted */
    FF_User1    = 0x00100000,  /*< Flag to be used by the user */
    FF_User2    = 0x00200000,  /*< Flag to be used by the user */
    FF_All      = 0xFFFFFFFF
};

enum class FaceHandleType
{
    NULLTYPE = 0,
    DEFAULT,
    GPUSUBDIVISION,
    
    MAX = 255
};

class FaceHandle {
public:
    typedef FaceHandle* 		shared_ptr;
    typedef FaceHandle* 		weak_ptr;
    typedef FaceHandle* 		Ptr;
    typedef uint32_t 			size_t;
    size_t _id;
    
    FaceHandle(FaceHandleType type = FaceHandleType::NULLTYPE)
    : _id( 0 | ((int)type << 24) )
    {}
    
    FaceHandle(const FaceHandle&) = default;
    
    FaceHandle(FaceHandle&&) = default;
    
    uint8_t type() const { return _id >> 24; }
    size_t 	iid() const { return _id & 0xFFFFFF; }
    
    void    setIid(size_t iid) {
        _id = (_id & 0xFF000000) | (iid & 0xFFFFFF);
    }
    
    template<typename T>
    T & cast() const {
        return static_cast<T>(*this);
    }
};

namespace core
{
namespace subdivision
{
class Edge;

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
class Face : public FaceHandle {
public:
    typedef Face* 		shared_ptr;
    typedef Face* 		weak_ptr;
    typedef Face* 		Ptr;
    
    typedef uint32_t 	size_t;
    
private:
    ISurface*           _surface;
    Edge*               _he;
    Vertex::weak_ptr    _vertex;
    FaceFlags           _flags;
    mutable size_t      _nVertices;
    void*               _userData;
    
public:
    /**
     * Default constructor. Initiliazes all point to index 0.
     */
    Face(ISurface *surface = NULL);

    /**
     * Construct a new triangle. The triangle is composed by the passed
     * points ids or references.
     *
     * @param p1 first point that form the triangle.
     * @param p2 second point that form the triangle.
     * @param p3 third point that form the triangle.
     */
    Face(ISurface *surface, const std::vector<int>& vertexIndexList);

    ~Face();

    /**
     * Get the hash code for this instance
     */
    inline unsigned long hashCode() const;

    /**
     * Set / get attribute flags to the face.
     *
     * @param flag possible values
     *              FF_None     
     *              FF_Selected 
     *              FF_Deleted  
     *              FF_All      
     */
    void addFlag(FaceFlags flag) { _flags = (FaceFlags)(_flags | flag); }
    void removeFlag(FaceFlags flag) { _flags = (FaceFlags)(_flags & ~flag); }
    FaceFlags flags() const { return _flags; }
    void setFlags(FaceFlags flags) {_flags = flags; }
    bool isFlagSet(FaceFlags flag) { return (_flags & flag) != 0;}

    /**
     * Checks if the triangle data is valid. Triangle is valid only if
     * the three point that compose it are different.
     *
     * @return true if triangle is valid. False, otherwise.
     */
    bool isValid();

    /**
     * Add a vertex to the face. This method allows adding multiple vertices
     * by chaining addVertex calls.
     */
    Face& addEdge(Edge* he);

    /**
     * Returns one vertex that belongs to this face.
     *
     * It can be used as staring poont for vertex iteration.
     */
    const Vertex* vertex() const { return _vertex ;}
    
    /**
     * Adds all the vertices contained in the array.
     */
    void addVertex(const std::vector<Vertex*> v);

    /**
     * Gets/sets a reference to a half edge that forms this face.
     */
    Edge* hedge() { return _he; }
    void setHEdge(Edge* hedge);

    /**
     * Returns the number of vertices in this face.
     */
    size_t numVertices() const;
    
    /**
     * Sets the first point index reference.
     *
     * @param p1 index of the first point.
     * @param p2 index of the second point.
     * @param p3 index of the third point.
     */
    void setPoints(const std::vector<size_t>& vertexIndexList);

    bool hasEdge(const Edge& /*e*/) const;

    bool hasEdge(size_t /*v1*/, size_t /*v2*/) const ;

    /**
     * Allows to attach a pointer to a user defined structure or data
     */
    void setUserData(void * data) {
        _userData = data;
    }
    
    /**
     * Returns a pointer to the user data.
     *
     * The user is responsible to cast the pointer to the correct type
     */
    void * userData() const {
        return _userData;
    }
    
    bool operator==(const Face& t) const;

    operator Point3() const ;
    operator Point3();

    /**
     * Gets an iterator object to traverse all the vertices that form part
     * of this face.
     */
    Iterator<VertexHandle> vertexIterator();

    /**
     * Gets an iterator object to traverse all the vertices that form part
     * of this face.
     */
    Iterator<VertexHandle> constVertexIterator() const;

    /**
     * Gets an iterator object to traverse all the edges that form part
     * of this face.
     */
    Iterator<EdgeHandle> edgeIterator();

    /**
     * Gets an iterator object to traverse all the edges that form part
     * of this face.
     */
    Iterator<EdgeHandle> constEdgeIterator() const;

    
protected:
    ISurface * surface() { return _surface; }
    void subdivide(int level);

    /* Inner classes */
    class VertexIterator;
    class EdgeIterator;

    friend class VertexIterator;
    friend class EdgeIterator;
};

}; // namespace subdivision
}; // namespace core

#endif
