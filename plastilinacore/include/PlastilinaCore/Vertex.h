/*
 *  Vertex.h
 *  QSculpt
 *
 *  Created by Roberto Cabral on 12/18/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef VERTEX_H
#define VERTEX_H

#include <PlastilinaCore/Color.h>
#include <PlastilinaCore/IIterator.h>
#include <PlastilinaCore/Point3D.h>
#include <Eigen/Core>
#include <atomic>

#define VERTEXTYPE_DEFAULT (0)
#define VERTEXTYPE_POS (1)
#define VERTEXTYPE_POS_NOR (2)
#define VERTEXTYPE_POS_NOR_COL (3)
#define VERTEXTYPE_POS_NOR_COL_TEX (4)

enum class VertexHandleType
{
    NULLTYPE = 0,
    DEFAULT,
    GPUSUBDIVISION,

    MAX = 255
};

class VertexHandle
{
  public:
    typedef VertexHandle*       shared_ptr;
    typedef const VertexHandle* const_shared_ptr;
    typedef VertexHandle*       weak_ptr;
    typedef VertexHandle*       ptr;
    typedef const VertexHandle* const_ptr;
    typedef uint32_t            size_t;
    size_t                      _id;

    VertexHandle(VertexHandleType type = VertexHandleType::NULLTYPE)
        : _id(0 | ((int)type << 24))
    {
    }

    VertexHandle(const VertexHandle&) = default;

    VertexHandle(VertexHandle&&) = default;

    uint8_t type() const { return _id >> 24; }
    size_t  iid() const { return _id & 0xFFFFFF; }

    void setIid(size_t iid) { _id = (_id & 0xFF000000) | (iid & 0xFFFFFF); }

    template <typename T>
    T cast() const
    {
        return static_cast<T>(this);
    }

    Vector4 position() const;
    Vector4 normal() const;
};

enum VertexFlags
{
    VF_None     = 0,
    VF_Selected = 0x00000001, /*< Vertex is selected*/
    VF_Deleted  = 0x00000002, /*< Vertex is marked as deleted.*/
    VF_Crease   = 0x00000004,
    VF_User1    = 0x00100000, /*< Flag to use for the user */
    VF_User2    = 0x00200000, /*< Flag to use for the user */
    VF_ALL      = 0xFFFFFFFF
};

namespace core
{
namespace subdivision
{
class Edge;
class Face;

class Vertex : public VertexHandle
{
  public:
    typedef Vertex*       shared_ptr;
    typedef const Vertex* const_shared_ptr;
    typedef Vertex*       weak_ptr;
    typedef Vertex*       ptr;
    typedef const Vertex* const_ptr;

    typedef VertexHandle::size_t size_t;

  private:
    static std::atomic_int NEXT_ID;

    Point3      _position;  // 12
    Vector3     _normal;    // 12
    Color       _color;     // 12
    Vector2     _texCoords; // 12
    VertexFlags _flags;     // 4
    Edge*       _he;        // 8
    void*       _userData;  // 8

  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    /**
     * Contructs a new vertex instance
     */
    Vertex();

    /**
     * Construct a new vertex with the position specified
     */
    Vertex(const Point3& position, const Vector3& normal = Vector3(0, 1, 0),
           const Color&  color     = Color(1, 1, 1, 1),
           const Point2& texCoords = Point2(0, 0));

    /**
     * Copy constructor. Creates a new instance of a vertex
     * with the same attributes as another vertex.
     *
     * The new vertex have the same half edge assigned as the original
     * vertex.
     */
    Vertex(const Vertex&);

    /**
     * Gets / sets the position of the vertex
     */
    Point3  position() const { return _position; }
    Point3& position() { return _position; }

    /**
     * Get / sets the color attribute of the vertex;
     */
    Color  color() const { return _color; }
    Color& color() { return _color; }

    /**
     * Gets / sets the normal attribute for this vertex instance
     */
    Vector3  normal() const { return _normal; }
    Vector3& normal() { return _normal; }

    /**
     * Gets / sets the texture coordinates attribute for this vertex instance
     */
    Vector2  texcoords() const { return _texCoords; }
    Vector2& texcoords() { return _texCoords; }

    /**
     * Assign or returns one of the half edges associated to the vertex.
     */
    void setEdge(Edge* he) { _he = he; }
    Edge*              edge() const { return _he; }

    /**
     * Set / get attribute flags to the vertex.
     */
    void addFlag(VertexFlags flag) { _flags = (VertexFlags)(_flags | flag); }
    void removeFlag(VertexFlags flag)
    {
        _flags = (VertexFlags)(_flags & ~flag);
    }
    VertexFlags flags() const { return _flags; }
    bool isFlagSet(VertexFlags flag) { return (_flags & flag) != 0; }

    /**
     * Allows to attach a pointer to a user defined structure or data
     */
    void setUserData(void* data) { _userData = data; }

    /**
     * Returns a pointer to the user data.
     *
     * The user is responsible to cast the pointer to the correct type
     */
    void* userData() const { return _userData; }

    /**
     * Operator to treat the Vertex class as a Point3 instance. Returns
     * the position of the vertex.
     */
    operator Point3() { return _position; };

    std::string toString();

    Vertex& operator=(const Vertex& v);

    /**
     * Equals operator.
     */
    bool operator==(const Vertex& v) const;

    /**
     * Equals operator.
     */
    bool operator!=(const Vertex& v) const;

    /**
     *
     */
    Iterator<Vertex> vertexIterator();

    /**
     * Used to iterate over faces that share this vertex.
     */
    Iterator<Face> faceIterator();

    /**
     * Used to iterate over all the edges that share the same point.
     */
    Iterator<Edge> edgeIterator();

  private:
    class VertexIterator;
    class FaceIterator;
    class EdgeIterator;

    friend class VertexIterator;
    friend class FaceIterator;
    friend class EdgeIterator;
};

} // namespace subdivision
} // namespace core

#endif
