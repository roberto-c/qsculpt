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
#include "GpuSubdivision.h"

#include "DocumentModelTest.h"
#include "PlastilinaCore/Logging.h"

using core::GpuSubdivision;

struct SubdivisionTest::Impl
{
    Document::shared_ptr doc;
    ISurface::shared_ptr surface;
    
    int
    setup();
    
    int
    test();
    
    int
    cleanup();
};

int SubdivisionTest::Impl::setup() {
    TRACEFUNCTION();
    using core::gpusubdivision::Vertex;
    using core::utils::convert_to;
    
    doc = std::make_shared<Document>();
    
    //ISurface * surf = new Box();
    surface = std::make_shared<GpuSubdivision>();
    double hw = 1.0;
    double hh = 1.0;
    double hd = 1.0;
    
    std::vector<size_t> vertexID(8);
    vertexID[0] = surface->addVertex(Point3(-hw, hh,-hd));
    vertexID[1] = surface->addVertex(Point3( hw, hh,-hd));
    vertexID[2] = surface->addVertex(Point3( hw,-hh,-hd));
    vertexID[3] = surface->addVertex(Point3(-hw,-hh,-hd));
    static_cast<Vertex*>(surface->vertex(vertexID[0]))->n = convert_to<cl_float4>(Vector3(-hw, hh,-hd).normalized());
    static_cast<Vertex*>(surface->vertex(vertexID[1]))->n = convert_to<cl_float4>(Vector3( hw, hh,-hd).normalized());
    static_cast<Vertex*>(surface->vertex(vertexID[2]))->n = convert_to<cl_float4>(Vector3( hw,-hh,-hd).normalized());
    static_cast<Vertex*>(surface->vertex(vertexID[3]))->n = convert_to<cl_float4>(Vector3(-hw,-hh,-hd).normalized());
    static_cast<Vertex*>(surface->vertex(vertexID[0]))->t = convert_to<cl_float2>(Point2(0,0));
    static_cast<Vertex*>(surface->vertex(vertexID[1]))->t = convert_to<cl_float2>(Point2(1,0));
    static_cast<Vertex*>(surface->vertex(vertexID[2]))->t = convert_to<cl_float2>(Point2(1,1));
    static_cast<Vertex*>(surface->vertex(vertexID[3]))->t = convert_to<cl_float2>(Point2(0,1));
    
    vertexID[4] = surface->addVertex(Point3(-hw, hh, hd));
    vertexID[5] = surface->addVertex(Point3( hw, hh, hd));
    vertexID[6] = surface->addVertex(Point3( hw,-hh, hd));
    vertexID[7] = surface->addVertex(Point3(-hw,-hh, hd));
    static_cast<Vertex*>(surface->vertex(vertexID[0]))->n = convert_to<cl_float4>(Vector3(-hw, hh, hd).normalized());
    static_cast<Vertex*>(surface->vertex(vertexID[1]))->n = convert_to<cl_float4>(Vector3( hw, hh, hd).normalized());
    static_cast<Vertex*>(surface->vertex(vertexID[2]))->n = convert_to<cl_float4>(Vector3( hw,-hh, hd).normalized());
    static_cast<Vertex*>(surface->vertex(vertexID[3]))->n = convert_to<cl_float4>(Vector3(-hw,-hh, hd).normalized());
    static_cast<Vertex*>(surface->vertex(vertexID[0]))->t = convert_to<cl_float2>(Point2(0,0));
    static_cast<Vertex*>(surface->vertex(vertexID[1]))->t = convert_to<cl_float2>(Point2(1,0));
    static_cast<Vertex*>(surface->vertex(vertexID[2]))->t = convert_to<cl_float2>(Point2(1,1));
    static_cast<Vertex*>(surface->vertex(vertexID[3]))->t = convert_to<cl_float2>(Point2(0,1));
    
    std::vector<Vertex::size_t> indexList(4);
    indexList[0] = vertexID[0];
    indexList[1] = vertexID[1];
    indexList[2] = vertexID[2];
    indexList[3] = vertexID[3];
    surface->addFace( indexList );
    indexList[0] = vertexID[4];
    indexList[1] = vertexID[7];
    indexList[2] = vertexID[6];
    indexList[3] = vertexID[5];
    surface->addFace( indexList );
    indexList[0] = vertexID[0];
    indexList[1] = vertexID[3];
    indexList[2] = vertexID[7];
    indexList[3] = vertexID[4];
    surface->addFace( indexList );
    indexList[0] = vertexID[5];
    indexList[1] = vertexID[6];
    indexList[2] = vertexID[2];
    indexList[3] = vertexID[1];
    surface->addFace( indexList );
    indexList[0] = vertexID[0];
    indexList[1] = vertexID[4];
    indexList[2] = vertexID[5];
    indexList[3] = vertexID[1];
    surface->addFace( indexList );
    indexList[0] = vertexID[7];
    indexList[1] = vertexID[3];
    indexList[2] = vertexID[2];
    indexList[3] = vertexID[6];
    surface->addFace( indexList );

    return 0;
}

int SubdivisionTest::Impl::test() {
    TRACEFUNCTION();
    using core::gpusubdivision::Vertex;
    using core::gpusubdivision::Face;
    
    int counter = 0;
    auto vtxIt = surface->constVertexIterator();
    while (vtxIt.hasNext()) {
        auto vertex = vtxIt.next();
        if (vertex->type() == (int)VertexHandleType::GPUSUBDIVISION) {
            counter++;
            Vertex *vtx = static_cast<Vertex*>(vertex);
            std::cout << "Vertex: " << vtx->p.s[0] << "," << vtx->p.s[1] << ","
                      << vtx->p.s[2] << "," << vtx->p.s[3] << "\n";
        }
    }
    std::cout << "Number of vertices: " << counter <<  std::endl;
    counter=0;
    auto faceIt = surface->constFaceIterator();
    while (faceIt.hasNext()) {
        auto face = faceIt.next();
        if (face->type() == (int)FaceHandleType::GPUSUBDIVISION) {
            counter++;
            Face *face = static_cast<Face*>(face);
            
        }
    }
    std::cout << "Number of faces: " << counter <<  std::endl;
    if (counter == 8) {
        return 0;
    }
    return 1;
}

int SubdivisionTest::Impl::cleanup() {
    TRACEFUNCTION();
    return 0;
}

SubdivisionTest::SubdivisionTest()
    : d_(new Impl())
{
}

SubdivisionTest::~SubdivisionTest()
{
}

int SubdivisionTest::run() {
    TRACEFUNCTION();
    if (d_->setup() != 0) {
        return -1;
    }
    int err = d_->test();
    d_->cleanup();
    
    return err;
}