/***************************************************************************
 *   Copyright (C) 2015 by Juan Roberto Cabral Flores                      *
 *   roberto.cabral@gmail.com                                              *
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
#include "stable.h"
#include <PlastilinaCore/Plastilina.h>
#include "PrimitiveFactory.h"

#include <PlastilinaCore/ISurface.h>
#include <PlastilinaCore/Logging.h>
#include <PlastilinaCore/subdivision/Subdivision.h>
#include <PlastilinaCore/opencl/CLUtils.h>
#include "GpuSubdivision.h"

namespace core {

GpuSubdivision *
PrimitiveFactory<GpuSubdivision>::createBox() {
    using core::gpusubdivision::Vertex;
    using core::gpusubdivision::Edge;
    using core::gpusubdivision::Face;
    using core::utils::convert_to;
    using core::utils::to_string;
    
    GpuSubdivision * surface = new GpuSubdivision;
    
    surface->lock();
    //qDebug("Box::initPoints()");
    float hw = 1.0f;
    float hh = 1.0f;
    float hd = 1.0f;
    
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
    static_cast<Vertex*>(surface->vertex(vertexID[4]))->n = convert_to<cl_float4>(Vector3(-hw, hh, hd).normalized());
    static_cast<Vertex*>(surface->vertex(vertexID[5]))->n = convert_to<cl_float4>(Vector3( hw, hh, hd).normalized());
    static_cast<Vertex*>(surface->vertex(vertexID[6]))->n = convert_to<cl_float4>(Vector3( hw,-hh, hd).normalized());
    static_cast<Vertex*>(surface->vertex(vertexID[7]))->n = convert_to<cl_float4>(Vector3(-hw,-hh, hd).normalized());
    static_cast<Vertex*>(surface->vertex(vertexID[4]))->t = convert_to<cl_float2>(Point2(0,0));
    static_cast<Vertex*>(surface->vertex(vertexID[5]))->t = convert_to<cl_float2>(Point2(1,0));
    static_cast<Vertex*>(surface->vertex(vertexID[6]))->t = convert_to<cl_float2>(Point2(1,1));
    static_cast<Vertex*>(surface->vertex(vertexID[7]))->t = convert_to<cl_float2>(Point2(0,1));

//    Iterator<VertexHandle> it = surface->vertexIterator();
//    while (it.hasNext()) {
//        static_cast<Vertex*>(it.next())->addFlag(VF_Crease);
//    }
    
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

    int counter = 0;
    auto vtxIt = surface->constVertexIterator();
    for (auto & vertex : vtxIt)
    {
        TRACE(debug) << "Vertex: " << vertex.iid();
        if (vertex.type() == (int)VertexHandleType::GPUSUBDIVISION) {
            Vertex *vtx = static_cast<Vertex*>(&vertex);
            TRACE(debug) << "  p: " << to_string(vtx->p);
        }
    }

    counter = 0;
    auto faceIt = surface->constFaceIterator();
    for (auto & face : faceIt)
    {
        TRACE(debug) << "Face: " << face.iid();
        if (face.type() == (int)FaceHandleType::GPUSUBDIVISION) {
            Face *f = static_cast<Face*>(&face);
            TRACE(debug) << "  FacexId: " << f->iid();
            TRACE(debug) << "  EdgeId: " << surface->edge(f->edge)->iid();
            auto faceVtxIt = f->constVertexIterator(surface);
            for (auto & vertex : faceVtxIt)
            {
                TRACE(debug) << "    Vertex: " << vertex.iid();
                if (vertex.type() == (int)VertexHandleType::GPUSUBDIVISION) {
                    Vertex *vtx = static_cast<Vertex*>(&vertex);
                    TRACE(debug) << "      p: " << to_string(vtx->p);
                }
            }
        }
    }

    surface->unlock();
    return surface;
}

Subdivision *
PrimitiveFactory<Subdivision>::createBox() {
    using core::subdivision::Vertex;
    
    Subdivision * surface = new Subdivision;
    
    //qDebug("Box::initPoints()");
    float hw = 1.0f;
    float hh = 1.0f;
    float hd = 1.0f;
    
    std::vector<size_t> vertexID(8);
    vertexID[0] = surface->addVertex(Point3(-hw, hh,-hd));
    vertexID[1] = surface->addVertex(Point3( hw, hh,-hd));
    vertexID[2] = surface->addVertex(Point3( hw,-hh,-hd));
    vertexID[3] = surface->addVertex(Point3(-hw,-hh,-hd));
    static_cast<Vertex*>(surface->vertex(vertexID[0]))->normal() = Vector3(-hw, hh,-hd).normalized();
    static_cast<Vertex*>(surface->vertex(vertexID[1]))->normal() = Vector3( hw, hh,-hd).normalized();
    static_cast<Vertex*>(surface->vertex(vertexID[2]))->normal() = Vector3( hw,-hh,-hd).normalized();
    static_cast<Vertex*>(surface->vertex(vertexID[3]))->normal() = Vector3(-hw,-hh,-hd).normalized();
    static_cast<Vertex*>(surface->vertex(vertexID[0]))->texcoords() = Point2(0,0);
    static_cast<Vertex*>(surface->vertex(vertexID[1]))->texcoords() = Point2(1,0);
    static_cast<Vertex*>(surface->vertex(vertexID[2]))->texcoords() = Point2(1,1);
    static_cast<Vertex*>(surface->vertex(vertexID[3]))->texcoords() = Point2(0,1);
    
    vertexID[4] = surface->addVertex(Point3(-hw, hh, hd));
    vertexID[5] = surface->addVertex(Point3( hw, hh, hd));
    vertexID[6] = surface->addVertex(Point3( hw,-hh, hd));
    vertexID[7] = surface->addVertex(Point3(-hw,-hh, hd));
    static_cast<Vertex*>(surface->vertex(vertexID[4]))->normal() = Vector3(-hw, hh, hd).normalized();
    static_cast<Vertex*>(surface->vertex(vertexID[5]))->normal() = Vector3( hw, hh, hd).normalized();
    static_cast<Vertex*>(surface->vertex(vertexID[6]))->normal() = Vector3( hw,-hh, hd).normalized();
    static_cast<Vertex*>(surface->vertex(vertexID[7]))->normal() = Vector3(-hw,-hh, hd).normalized();
    static_cast<Vertex*>(surface->vertex(vertexID[4]))->texcoords() = Point2(0,0);
    static_cast<Vertex*>(surface->vertex(vertexID[5]))->texcoords() = Point2(1,0);
    static_cast<Vertex*>(surface->vertex(vertexID[6]))->texcoords() = Point2(1,1);
    static_cast<Vertex*>(surface->vertex(vertexID[7]))->texcoords() = Point2(0,1);
    
    Iterator<VertexHandle> it = surface->vertexIterator();
    while (it.hasNext()) {
        static_cast<Vertex*>(it.next())->addFlag(VF_Crease);
    }
    
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
    
    return surface;
}

    
}; /* namespace core */
