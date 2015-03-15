//
//  SubdivisionTest.cpp
//  qsculpt
//
//  Created by Juan Cabral on 6/21/12.
//  Copyright (c) 2012 plastilinaware. All rights reserved.
//

#include <iostream>

#include "SubdivisionTest.h"

#include "PlastilinaCore/ISurface.h"
#include "PlastilinaCore/Subdivision/Subdivision.h"
#include "PlastilinaCore/Subdivision/Box.h"
#include "PlastilinaCore/Subdivision/Sphere.h"

#include "DocumentModelTest.h"
#include "PlastilinaCore/Logging.h"

struct NIID {
    typedef unsigned long size_t;
    
    //size_t iid;
    size_t iid() {
        ::size_t val = (::size_t)this;
        return (unsigned long)(val >> 4) & 0xFFFFFFFF;
    }
};

struct NVertex : public NIID {
    typedef NIID::size_t size_t;
    
    Eigen::Vector4f p;
};

struct NEdge : public NIID {
    typedef NIID::size_t size_t;
    size_t head;
    size_t tail;
    size_t next;
    size_t pair;
    size_t face;
};

struct NFace : public NIID {
    typedef NIID::size_t size_t;
    
    size_t vtx;
    size_t edge;
};

template <class VertexContainerType, class EdgeContainerType, class FaceContainerType>
struct VertexInFaceIterator {
    VertexContainerType * vertex;
    EdgeContainerType * edge;
    FaceContainerType * face;
    NFace::size_t faceHnd;
    NEdge::size_t e = 0, startEdge;
    
    VertexInFaceIterator(VertexContainerType * vertex,
                         EdgeContainerType * edge,
                         FaceContainerType * face,
                         NFace::size_t faceHnd)
    : vertex(vertex), edge(edge), face(face), faceHnd(faceHnd)
    {
        startEdge = face->at(faceHnd - 1).edge;
    }
    
    bool hasNext() {
        bool ret = false;
        ret = startEdge && (e != startEdge);
        return ret;
    }
    
    NVertex::size_t next() {
        if (e == 0) e = startEdge;
        NVertex::size_t ret = edge->at(e-1).head;
        e = edge->at(e - 1).next;
        return ret;
    }
};

struct NSubdivision {
    std::vector<NVertex>    vertex_;
    std::vector<NEdge>      edge_;
    std::vector<NFace>      face_;
    
    NVertex & vertex(NVertex::size_t index) {
        return vertex_[index - 1];
    }
    
    NEdge & edge(NEdge::size_t index) {
        return edge_[index - 1];
    }
    
    NFace & face(NFace::size_t index) {
        return face_[index - 1];
    }
    
    void initPoints() {
        std::vector<NVertex::size_t> points;
        NVertex::size_t vid = 0;
        vid = addVertex(Point3(-1, -1, 0) );
        points.push_back(vid);
        vid = addVertex(Point3( 1, -1, 0) );
        points.push_back(vid);
        vid = addVertex(Point3( 1,  1, 0) );
        points.push_back(vid);
        vid = addVertex(Point3(-1,  1, 0) );
        points.push_back(vid);
        
        addFace(points[0], points[1], points[2], points[3]);
    }
    
    NFace::size_t addFace(NVertex::size_t v1,
                          NVertex::size_t v2,
                          NVertex::size_t v3,
                          NVertex::size_t v4)
    {
        NFace f;
        face_.push_back(f);
        NFace::size_t id = face_.size();
        
        NEdge::size_t e1 = addEdge(v1, v2);
        NEdge::size_t e2 = addEdge(v2, v3);
        NEdge::size_t e3 = addEdge(v3, v4);
        NEdge::size_t e4 = addEdge(v4, v1);
        
        edge(e1).next = e2;
        edge(e1).face = id;
        edge(e2).next = e3;
        edge(e2).face = id;
        edge(e3).next = e4;
        edge(e3).face = id;
        edge(e4).next = e1;
        edge(e4).face = id;
                
        face(id).edge = e1;
        
        return id;
    }
    
    NEdge::size_t addEdge(NVertex::size_t head,
                          NVertex::size_t tail)
    {
        NEdge e;
        edge_.push_back(e);
        NEdge::size_t id = edge_.size();
        edge(id).head = head;
        edge(id).tail = tail;
        return id;
    }
    
    NVertex::size_t addVertex(const Point3 & p) {
        NVertex v;
        v.p = Eigen::Vector4f(p[0],p[1],p[2],1.0);
        vertex_.push_back(v);
        return vertex_.size();
    }
    
    
    
    void subdivide() {
        NVertex::size_t nvtx = 0;
        Point4 p;
        for (NFace::size_t i = 1; i <= face_.size(); ++i) {
            nvtx = 0;
            VertexInFaceIterator<std::vector<NVertex>,std::vector<NEdge>,std::vector<NFace> >
                it(&vertex_, &edge_, &face_, i);
            
            while(it.hasNext()) {
                auto vtx = it.next();
                p += vertex(vtx).p;
                std::cerr << "NVertex[" << nvtx << "]= " << toString(vertex(vtx).p);
                nvtx++;
            }
            p /= nvtx;
            std::cerr << "Mid point: " << toString(p);
        }
    }
};

static void printModelRecursive(const Document::shared_ptr & doc, 
                                const SceneNode::shared_ptr & parent = nullptr)
{
    TRACEFUNCTION();
    
    Logging_increaseIndentation();
    size_t numrows = doc->childrenCount(parent);
    for(int i = 0; i < numrows; ++i) {
        auto index = doc->index(i, parent);
        std::cerr << "iid: " << index->iid();
        printModelRecursive(doc, index);
    }
    Logging_decreaseIndentation();
}


int SubdivisionTest::setup() {
    TRACEFUNCTION();
    doc = std::make_shared<Document>();
    
    //ISurface * surf = new Box();
    ISurface * surf = new Sphere();
    
    node1 = std::make_shared<SurfaceNode>(surf);
    node1->transform().translate(Vector3(1, 0, 0));
    node1->setName("Node 1");
    
    doc->addItem(node1);
    
    return 0;
}

int SubdivisionTest::test() {
    TRACEFUNCTION();
    // Print points in nodes:
    auto scene = doc->scene().lock();
    //    printSceneNode(scene);
    
    Subdivision * surface = static_cast<Subdivision*>(node1->surface());
    assert(surface);
    
    std::cerr << "Sizeof(NIID): " << sizeof(NIID) << std::endl;
    std::cerr << "Sizeof(NVertex): " << sizeof(NVertex)<< std::endl;
    std::cerr << "Sizeof(NEdge): " << sizeof(NEdge)<< std::endl;
    std::cerr << "Sizeof(NFace): " << sizeof(NFace)<< std::endl;
    std::cerr << std::endl;
    
    std::cerr << "Vertex size: " << sizeof(Vertex)<< std::endl;
    std::cerr << "Face size: " << sizeof(Face)<< std::endl;
    std::cerr << "int size: " << sizeof(int)<< std::endl;
    std::cerr << "size_t size: " << sizeof(size_t)<< std::endl;
    std::cerr << "ptr size: " << sizeof(void*)<< std::endl;
    std::cerr << "Point3: " << sizeof(Point3)<< std::endl;
    std::cerr<< std::endl;
    
    //return 0;
    
    std::cerr << "Vertex: " << surface->numVertices()<< std::endl;
    std::cerr << "Faces: " << surface->numFaces()<< std::endl;
    surface->addResolutionLevel();
    std::cerr << "Vertex: " << surface->numVertices()<< std::endl;
    std::cerr << "Faces: " << surface->numFaces()<< std::endl;
    surface->addResolutionLevel();
    std::cerr << "Vertex: " << surface->numVertices()<< std::endl;
    std::cerr << "Faces: " << surface->numFaces()<< std::endl;
    surface->addResolutionLevel();
    std::cerr << "Vertex: " << surface->numVertices()<< std::endl;
    std::cerr << "Faces: " << surface->numFaces()<< std::endl;
    surface->addResolutionLevel();
    std::cerr << "Vertex: " << surface->numVertices()<< std::endl;
    std::cerr << "Faces: " << surface->numFaces()<< std::endl;
    surface->addResolutionLevel();
    std::cerr << "Vertex: " << surface->numVertices()<< std::endl;
    std::cerr << "Faces: " << surface->numFaces()<< std::endl;
    surface->addResolutionLevel();
    std::cerr << "Vertex: " << surface->numVertices()<< std::endl;
    std::cerr << "Faces: " << surface->numFaces()<< std::endl;
    
    printModelRecursive(doc);
    return 0;
}

int SubdivisionTest::cleanup() {
    TRACEFUNCTION();
    return 0;
}

int SubdivisionTest::run() {
    TRACEFUNCTION();
    if (setup() != 0) {
        return -1;
    }
    int err = test();
    cleanup();
    
    return err;
}