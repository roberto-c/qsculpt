#include <PlastilinaCore/Stable.h>
#include <PlastilinaCore/BOManager.h>
#include <PlastilinaCore/Color.h>
#include <PlastilinaCore/ISurface.h>
#include <PlastilinaCore/Material.h>
#include <PlastilinaCore/Scene.h>
#include <PlastilinaCore/WireframeRenderer.h>
#include <PlastilinaCore/opengl/GlslProgram.h>
#include <PlastilinaCore/opengl/GlslShader.h>
#include <PlastilinaCore/opengl/VertexArrayObject.h>
#include <cstddef>
#define BO_POOL_NAME "WireframeRendererPool"

using core::subdivision::Vertex;
using core::subdivision::Face;

static Eigen::Vector4f g_selectedColor(1.0f, 0.0f, 0.0f, 1.0f);
static Eigen::Vector4f g_normalColor(0.8f, 0.8f, 0.8f, 1.0f);

typedef struct tagFlatVtxStruct
{
    GLfloat v[4];
    GLfloat n[4];
    GLfloat c[4];

    static tagFlatVtxStruct create(Eigen::Vector3f& vtx, Eigen::Vector3f& nor,
                                   Eigen::Vector4f& col)
    {
        tagFlatVtxStruct ret;
        ret.setData(vtx, nor, col);
        return ret;
    }

    void setData(Eigen::Vector3f& vtx, Eigen::Vector3f& nor,
                 Eigen::Vector4f& col)
    {

        v[0] = vtx[0];
        v[1] = vtx[1];
        v[2] = vtx[2];
        v[3] = 1.0;
        n[0] = nor[0];
        n[1] = nor[1];
        n[2] = nor[2];
        n[3] = 0.0;
        c[0] = col[0];
        c[1] = col[1];
        c[2] = col[2];
        c[3] = col[3];
    }
} FlatVtxStruct;

class RendererPrivate
{
  public:
    /**
     * Draw the mesh using OpenGL VBOs.
     * The VBOs are re-build when the mesh has been changed since the last
     * draw.
     */
    static void renderObject(std::shared_ptr<SceneNode>& node);

    static VAO* getVAO(ISurface* mesh);
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
    std::cerr << "WireframeRenderer constructor" << std::endl;
}

WireframeRenderer::~WireframeRenderer()
{
    std::cerr << "WireframeRenderer destructor" << std::endl;
}

void WireframeRenderer::renderObject(std::shared_ptr<SceneNode>& node)
{
    RendererPrivate::renderObject(node);
}

void RendererPrivate::renderObject(std::shared_ptr<SceneNode>& node)
{
    ISurface*                 obj = NULL;
    std::shared_ptr<Material> mat;

    if (!node)
    {
        return;
    }
    SurfaceNode::shared_ptr snode =
        std::dynamic_pointer_cast<SurfaceNode>(node);
    if (!snode)
    {
        std::cerr << __func__ << ": Node is not a SurfaceNode.\n";
        return;
    }
    obj = snode->surface();
    mat = snode->material();

    // std::cerr << "Render as selected = " << mesh->getShowBoundingBox();
    if (obj == NULL || mat == NULL)
        return;

    VertexBuffer* vbo = getVBO(obj);
    if (vbo == NULL || vbo->objectID() == 0)
    {
        std::cerr << "Failed to create VBO. Fallback to immediate mode"
                  << std::endl;
        return;
    }

    VAO* vao = getVAO(obj);
    if (vao == NULL || vao->objectID() == 0)
    {
        std::cerr << "Failed to create VAO." << std::endl;
        return;
    }

    // Set the depth function to the correct value
    glDepthFunc(GL_LESS);

    vao->bind();
    vbo->bind();
    if (vbo->needUpdate())
    {
        fillVertexBuffer(obj, vbo);
        vbo->setNeedUpdate(false);

        GLint attColor = mat->shaderProgram()->attributeLocation("glColor");
        if (attColor >= 0)
        {
            glEnableVertexAttribArray(attColor);
            glVertexAttribPointer(attColor, 4, GL_FLOAT, GL_FALSE,
                                  sizeof(FlatVtxStruct),
                                  (GLvoid*)offsetof(FlatVtxStruct, c));
        }
        GLint attVtx = mat->shaderProgram()->attributeLocation("glVertex");
        if (attVtx >= 0)
        {
            glEnableVertexAttribArray(attVtx);
            glVertexAttribPointer(attVtx, 4, GL_FLOAT, GL_FALSE,
                                  sizeof(FlatVtxStruct),
                                  (GLvoid*)offsetof(FlatVtxStruct, v));
        }
        GLint attNormal = mat->shaderProgram()->attributeLocation("glNormal");
        if (attNormal >= 0)
        {
            glEnableVertexAttribArray(attNormal);
            glVertexAttribPointer(attNormal, 4, GL_FLOAT, GL_FALSE,
                                  sizeof(FlatVtxStruct),
                                  (GLvoid*)offsetof(FlatVtxStruct, n));
        }
        THROW_IF_GLERROR("Failed to get attribute");
    }

    mat->shaderProgram()->useProgram();
    GLsizei numVertices = vbo->getBufferSize() / sizeof(FlatVtxStruct);
    glDrawArrays(GL_LINES, 0, numVertices);

    vao->unbind();
}

VAO* RendererPrivate::getVAO(ISurface* mesh)
{
    VAO* vao = NULL;
    vao      = BOManager::getInstance()->getVAO(BO_POOL_NAME, mesh);
    if (vao == NULL)
    {
        vao = BOManager::getInstance()->createVAO(BO_POOL_NAME, mesh);
    }
    return vao;
}

VertexBuffer* RendererPrivate::getVBO(ISurface* mesh)
{
    VertexBuffer* vbo = NULL;
    vbo               = BOManager::getInstance()->getVBO(BO_POOL_NAME, mesh);
    if (vbo == NULL)
    {
        vbo = BOManager::getInstance()->createVBO(BO_POOL_NAME, mesh);
    }
    return vbo;
}

void RendererPrivate::fillVertexBuffer(ISurface* mesh, VertexBuffer* vbo)
{
    // std::cerr << "FlatRenderer::fillVertexBuffer Start time:" <<
    // QDateTime::currentDateTime();
    if (mesh == NULL || vbo->objectID() == 0)
        return;

    size_t numFaces = mesh->numFaces();
    if (numFaces == 0)
        return;

    std::vector<FlatVtxStruct> vtxData;
    vtxData.reserve(numFaces * 4);

    Iterator<FaceHandle> it = mesh->constFaceIterator();
    while (it.hasNext())
    {
        auto            f = static_cast<Face*>(it.next());
        Eigen::Vector4f color =
            f->flags() & FF_Selected ? g_selectedColor : g_normalColor;

        Vertex::shared_ptr     first = NULL, prev = NULL, v = NULL;
        Iterator<VertexHandle> vtxIt = f->constVertexIterator();
        if (!vtxIt.hasNext())
        {
            break;
        }
        first = prev = static_cast<Vertex*>(vtxIt.next());
        while (vtxIt.hasNext())
        {
            v = static_cast<Vertex*>(vtxIt.next());
            vtxData.push_back(FlatVtxStruct::create(prev->position(),
                                                    prev->normal(), color));
            vtxData.push_back(
                FlatVtxStruct::create(v->position(), v->normal(), color));
            std::swap(prev, v);
        }
        if (first && prev)
        {
            vtxData.push_back(FlatVtxStruct::create(prev->position(),
                                                    prev->normal(), color));
            vtxData.push_back(FlatVtxStruct::create(first->position(),
                                                    first->normal(), color));
        }
    }
    GLuint bufferSize =
        static_cast<GLuint>(vtxData.size() * sizeof(FlatVtxStruct));
    vbo->setBufferData((GLvoid*)vtxData.data(), bufferSize);
}