#include "StdAfx.h"
#include <QtOpenGL>
#include <QPolygon>
#include "WireframeRenderer.h"
#include "ISurface.h"


WireframeRenderer::WireframeRenderer()
{
    qDebug() << "WireframeRenderer constructor";
}

WireframeRenderer::~WireframeRenderer()
{
    qDebug() << "WireframeRenderer destructor";
}

void WireframeRenderer::renderObject(const ISurface* mesh)
{
    glColor3f(0.0f, 0.0f, 1.0f);
    Iterator<Face> it = mesh->constFaceIterator();
    while(it.hasNext()) {
        const Face& f = it.next();
        Iterator<Vertex> vtxIt = f.constVertexIterator();
        glBegin(GL_LINE_LOOP);
        while(vtxIt.hasNext()) {
            const Vertex& v = vtxIt.next();
            // qDebug() << "Vertex:" << toString(v.position());
            glVertex3f(v.position().x(),
                       v.position().y(),
                       v.position().z());
        }
        glEnd();
    }
}
