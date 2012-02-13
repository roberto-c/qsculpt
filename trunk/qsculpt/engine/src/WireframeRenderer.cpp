#include "StdAfx.h"
#include <QtOpenGL/QtOpenGL>
#include <QtCore/QMap>
#include <cstddef>
#include "WireframeRenderer.h"
#include "ISurface.h"
#include "BOManager.h"

#define BO_POOL_NAME "WireframeRendererPool"

static GLfloat g_selectedColor[] = {1.0f, 0.0f, 0.0f, 1.0f};
static GLfloat g_normalColor[] =   {0.8f, 0.8f, 0.8f, 1.0f};

typedef struct tagFlatVtxStruct
{
    GLfloat v[3];
    GLfloat n[3];
    GLfloat color[4];
} FlatVtxStruct;

class RendererPrivate
{
public:
    /**
     * Draw the mesh using OpenGL VBOs.
     * The VBOs are re-build when the mesh has been changed since the last draw.
     */
    static void renderVbo(const ISurface* mesh);

    /**
     * Draw the mesh using the glBeing()/glEnd() and friends functions.
     * This method is a fallback method if the  VBOs are not supported.
     */
    static void renderImmediate(const ISurface* mesh);

    /**
     *
     */
    static VertexBuffer* getVBO(ISurface* mesh);

    /**
     * Fill vertex buffer with the mesh data.
     */
    static void fillVertexBuffer(ISurface* mesh, VertexBuffer* vbo);
};

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
    RendererPrivate::renderVbo(mesh);
}

void RendererPrivate::renderImmediate(const ISurface* mesh)
{
    //qDebug() << "Render as selected = " << mesh->getShowBoundingBox();
    if (mesh == NULL)
        return;

    QColor color = Qt::white; //mesh->getPointList().at(f.normal[j]).color;
    if (mesh->isSelected())
    {
        glColor3d(color.redF(), color.greenF() + 0.3, color.blueF());
    }
    else
    {
        glColor3d(color.redF(), color.greenF(), color.blueF());
    }
    
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

void RendererPrivate::renderVbo(const ISurface* mesh)
{
    //qDebug() << "Render as selected = " << mesh->getShowBoundingBox();
    if (mesh == NULL)
        return;
    
    ISurface* obj = const_cast<ISurface*>(mesh);
    
    VertexBuffer* vbo = getVBO(obj);
    if (vbo == NULL || vbo->getBufferID() == 0)
    {
        qDebug() << "Failed to create VBO. Fallback to immediate mode" ;
        renderImmediate(mesh);
        return;
    }
    // Set the depth function to the correct value
    glDepthFunc(GL_LESS);
    
    if ( vbo->needUpdate())
    {
        fillVertexBuffer(obj, vbo);
        vbo->setNeedUpdate(false);
    }
    
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo->getBufferID());
    glColorPointer(4, GL_FLOAT, sizeof(FlatVtxStruct), (GLvoid*)offsetof(FlatVtxStruct, color));
    glNormalPointer(GL_FLOAT, sizeof(FlatVtxStruct), (GLvoid*)offsetof(FlatVtxStruct, n));
    glVertexPointer(3, GL_FLOAT, sizeof(FlatVtxStruct), (GLvoid*)offsetof(FlatVtxStruct, v));
    
    QColor color = Qt::white;
    if (mesh->isSelected())
        glColor3d(color.redF(), color.greenF() + 0.3, color.blueF());
    else
        glColor3d(color.redF(), color.greenF(), color.blueF());
    
    //qDebug() << "Draw mesh";
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDrawArrays(GL_QUADS, 0, obj->numFaces()*4);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    
    //qDebug() << "Mesh rendered";
}

VertexBuffer* RendererPrivate::getVBO(ISurface* mesh)
{
    VertexBuffer* vbo = NULL;
    vbo = BOManager::getInstance()->getVBO(BO_POOL_NAME, mesh);
    if (vbo == NULL)
    {
        vbo = BOManager::getInstance()->createVBO(BO_POOL_NAME, mesh);
    }
    return vbo;
}

void RendererPrivate::fillVertexBuffer(ISurface* mesh, VertexBuffer* vbo)
{
    //qDebug() << "FlatRenderer::fillVertexBuffer Start time:" << QDateTime::currentDateTime();
    if (mesh == NULL || vbo->getBufferID() == 0)
        return;
    
    int numFaces = mesh->numFaces();
    if (numFaces == 0)
        return;
    
    int numVertices = numFaces*4;
    FlatVtxStruct* vtxData = new FlatVtxStruct[numVertices];
    
    //    int fcounter = 0;
    int offset = 0;
    Iterator<Face> it = mesh->constFaceIterator();
    while(it.hasNext()) {
        const Face& f = it.next();
        //        qDebug() << "face " << fcounter++;
        Iterator<Vertex> vtxIt = f.constVertexIterator();
        while(vtxIt.hasNext()) {
            const Vertex& v = vtxIt.next();
            vtxData[offset].v[0] = v.position().x();
            vtxData[offset].v[1] = v.position().y();
            vtxData[offset].v[2] = v.position().z();
            
            vtxData[offset].n[0] = v.normal().x();
            vtxData[offset].n[1] = v.normal().y();
            vtxData[offset].n[2] = v.normal().z();
            
            if (f.flags() & FF_Selected) {
                memcpy(vtxData[offset].color, g_selectedColor,
                       sizeof(g_selectedColor)) ;
            }
            else
            {
                memcpy(vtxData[offset].color, g_normalColor,
                       sizeof(g_normalColor)) ;
            }
            offset++;
        }
    }
    vbo->setBufferData((GLvoid*)vtxData, numVertices*sizeof(FlatVtxStruct));
    
    delete [] vtxData;
    
    //qDebug() << "FlatRenderer::fillVertexBuffer End time:" << QDateTime::currentDateTime();
}