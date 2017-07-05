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
#include "Stable.h"
#include "command/SubdivideCommand.h"
#include <QtCore/QThread>
#include <QtWidgets/QProgressDialog>
#include <PlastilinaCore/Face.h>
#include <PlastilinaCore/HEdge.h>
#include <PlastilinaCore/IDocument.h>
#include <PlastilinaCore/ISurface.h>
#include <PlastilinaCore/PointRenderer.h>
#include <PlastilinaCore/subdivision/Subdivision.h>

#include "DocumentView.h"
#include "QSculptApp.h"
#include "QSculptWindow.h"

typedef QHash<std::pair<int, int>, Vertex*> MidEdgeMap;

// Vertex flags:
// VF_User1 => True if the vertex is an original point (was before the
//             subdivision iteration)

struct SubdivideCommand::Impl
{
    MidEdgeMap edgeMidPoint;

    Impl() {}
    Impl(const Impl& cpy) {}

    void splitEdges(ISurface& s);

    void addFaceMidPointVertex(ISurface& s);

    void createFaces(ISurface& s);

    void removeOldFaces(ISurface& s);

    void computeNewPosition(ISurface& s);

    //    void computeBorderPosition(ISurface& s,

    void cleanUserData(ISurface& s);

    void smoothNormals(ISurface& s);

    Vector3 computeFaceNormal(FaceHandle::weak_ptr f);

    void diagnostiscs(ISurface& s);
};

SubdivideCommand::SubdivideCommand()
    : CommandBase("Subdivide")
{
}

SubdivideCommand::SubdivideCommand(const SubdivideCommand& cpy)
    : CommandBase(cpy)
    , _d(new Impl(*cpy._d.data()))
{
}

SubdivideCommand::~SubdivideCommand() {}

ICommand* SubdivideCommand::clone() const
{
    return new SubdivideCommand(*this);
}

void SubdivideCommand::undo() {}

void SubdivideCommand::redo() {}

void SubdivideCommand::execute() {}

void SubdivideCommand::Impl::addFaceMidPointVertex(ISurface& s) {}

void SubdivideCommand::Impl::splitEdges(ISurface& s) {}

void SubdivideCommand::Impl::createFaces(ISurface& s) {}

void SubdivideCommand::Impl::computeNewPosition(ISurface& s) {}

void SubdivideCommand::Impl::removeOldFaces(ISurface& s) {}

void SubdivideCommand::Impl::cleanUserData(ISurface& s) {}

void SubdivideCommand::Impl::smoothNormals(ISurface& s) {}

Vector3 SubdivideCommand::Impl::computeFaceNormal(FaceHandle::weak_ptr face)
{
    return Vector3();
}

void SubdivideCommand::Impl::diagnostiscs(ISurface& s) {}

struct EditSubdivideCommand::Impl
{
    SurfaceNode::shared_ptr surf;
    PointRenderer           renderer;
    uint32_t                vtxIID;
    Point3                  mouseScreen;

    Impl()
        : surf(NULL)
        , vtxIID(0)
    {
        renderer.setPointSize(10.0f);
    }

    Impl(const Impl& cpy)
        : surf(cpy.surf)
        , vtxIID(cpy.vtxIID)
    {
        renderer.setPointSize(10.0f);
    }
};

EditSubdivideCommand::EditSubdivideCommand()
    : d_(new Impl)
{
}

EditSubdivideCommand::EditSubdivideCommand(const EditSubdivideCommand& cpy)
    : d_(new Impl(*cpy.d_.data()))
{
}

EditSubdivideCommand::~EditSubdivideCommand() {}

ICommand* EditSubdivideCommand::clone() const
{
    return new EditSubdivideCommand(*this);
}

void EditSubdivideCommand::execute() {}
void EditSubdivideCommand::undo() {}
void EditSubdivideCommand::redo() {}

void EditSubdivideCommand::mousePressEvent(QMouseEvent* e) {}

void EditSubdivideCommand::mouseReleaseEvent(QMouseEvent* e) {}

void EditSubdivideCommand::mouseMoveEvent(QMouseEvent* e) {}

void EditSubdivideCommand::paintGL(GlCanvas* c) {}
