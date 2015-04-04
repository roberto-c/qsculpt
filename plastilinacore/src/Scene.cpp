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
#include <PlastilinaCore/Stable.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>          // Output data structure
#include <assimp/mesh.h>
#include <assimp/postprocess.h>    // Post processing flags
#include <assimp/camera.h>

#include <PlastilinaCore/Camera.h>
#include <PlastilinaCore/IRenderer.h>
#include <PlastilinaCore/Scene.h>
#include <PlastilinaCore/SceneNode.h>
#include <PlastilinaCore/geometry/Ray.h>
#include <PlastilinaCore/geometry/Aabb.h>
#include <PlastilinaCore/Material.h>
#include <PlastilinaCore/material/PhongMaterial.h>
#include <PlastilinaCore/material/PointMaterial.h>
#include <PlastilinaCore/Octree.h>
#include <PlastilinaCore/subdivision/Subdivision.h>

//namespace  {
    struct CenterMassFn {
        Vector3 operator()(SceneNode::weak_ptr) {
            return Vector3();
        }
    };
//};

namespace  {
    float xfov_to_yfov_deg(float xfov, float aspect) {
        xfov = xfov * 2 * M_PI / 360;
        float yfov = 2.0f * atanf(tan(xfov * 0.5f) / aspect);
        return yfov * 360 / 2 / M_PI;
    }
    float yfov_to_xfov_deg(float yfov, float aspect) {
        yfov = yfov * 2 * M_PI / 360;
        float xfov = 2.0f * atanf(tan(yfov * 0.5f) * aspect);
        return xfov  * 360 / 2 / M_PI;
    }
}

struct Scene::Impl {
    //data::Octree<SceneNode::weak_ptr,CenterMassFn> octree;
    std::vector<aiNode*> cameraNodes;
    
    UpAxis	upAxis;
    
    Impl() : upAxis(UpAxis::Y_POS_UP)
    {
    }
    
    SceneNode::shared_ptr findByIidRecursive(SceneNode::const_shared_ptr root,
                                             uint IID) const;
	
	void renderRecursive(RenderState & state, const SceneNode * root) const;
    
    void importScene(const aiScene * scene, SceneNode::shared_ptr & outScene);
    
    void processNode(const aiScene * scene,
                     const aiNode * node,
                     SceneNode::shared_ptr & outNode);
    
    void processMeshes(const aiScene * scene,
                       const aiNode * node,
                       SceneNode::shared_ptr & outNode);
    void processCamera(const aiScene * scene,
                       const aiNode * node,
                       const aiCamera *camera,
                       SceneNode::shared_ptr & outNode);
    
    void processMaterial(const aiScene * scene,
                       const aiNode * node,
                       SceneNode::shared_ptr & outNode);
};

SceneNode::shared_ptr
Scene::Impl::findByIidRecursive(SceneNode::const_shared_ptr root,
                                                      uint IID) const
{
    auto it = root->constIterator();
    while (it.hasNext()) {
        auto e = it.next();
        if (e->iid() == IID) {
            return e;
        }
        e = findByIidRecursive(e, IID);
        if (e) {
            return e;
        }
    }
    return NULL;
}

void
Scene::Impl::renderRecursive(RenderState & state, const SceneNode * root) const
{
	Iterator<SceneNode> it = root->constIterator();
	while (it.hasNext()) {
		auto n = it.next();
		auto s = n ? std::dynamic_pointer_cast<SurfaceNode>(n) : nullptr;
		if (s) {
			state.currentNode = n.get();
			s->render(&state);
		}
		renderRecursive(state, n.get());
	}
}

Scene::Scene() : SceneNode(), _d(new Impl())
{
	std::cerr << __PRETTY_FUNCTION__ << std::endl;
}

Scene::Scene(const std::string& name): SceneNode(name.c_str()), _d(new Impl())
{
	std::cerr << __PRETTY_FUNCTION__ << " Name: " << name << std::endl;
}


Scene::~Scene()
{
	std::cerr << __PRETTY_FUNCTION__ << " Name: " << name() << std::endl;
}

UpAxis Scene::upAxis() const
{
    return _d->upAxis;
}

void Scene::setUpAxis(UpAxis axis)
{
    _d->upAxis = axis;
}

SceneNode::shared_ptr Scene::findByName(const std::string& name) const
{
    return NULL;
}

SceneNode::shared_ptr Scene::findByIID(uint IID) const
{
    auto thisptr = shared_from_this();
    return _d->findByIidRecursive(thisptr, IID);
}

bool Scene::intersects(const geometry::Ray &ray, 
                       data::ICollection<SceneNode::weak_ptr> *col)
{
    return false;
    //return _d->octree.findIntersect(ray, col);
}

bool Scene::intersects(const geometry::AABB &box, 
                       data::ICollection<SceneNode::weak_ptr> *col)
{
    return false;
    //return _d->octree.findIntersect(box, col);
}

void Scene::render() const
{
	try {
		RenderState state;
		state.camera = getCamera().get();
		state.root = this;
		state.currentNode = this;
		state.renderMode = RM_Smooth;
//		state.renderMode = RM_Points;
		_d->renderRecursive(state,this);
    } catch(core::GlException & e) {
        std::cerr   << "GLException: " << e.what() << std::endl
        << e.error() << ": " << e.errorString() << std::endl;
    }
}

void Scene::render(const RenderState * state) const
{
    
}

CameraNode::shared_ptr Scene::createCamera()
{
	std::shared_ptr<Camera> cam = std::make_shared<Camera>();
	return std::make_shared<CameraNode>(cam);
}

void Scene::loadFromFile(const std::string & filename)
{
    // Import 3D library assets
    // Start the import on the given file with some example postprocessing
    // Usually - if speed is not the most important aspect for you - you'll t
    // probably to request more postprocessing than we do in this example.
    Assimp::Importer importer;
    const aiScene * scene = importer.ReadFile( filename,
                                        aiProcess_CalcTangentSpace       |
                                        aiProcess_Triangulate            |
                                        aiProcess_JoinIdenticalVertices  |
                                        aiProcess_SortByPType);
    // If the import failed, report it
    if( !scene)
    {
        throw std::runtime_error(importer.GetErrorString());
    }
    auto rootNode = this->shared_from_this();
    _d->importScene(scene, rootNode);
}

void Scene::Impl::importScene(const aiScene * scene, SceneNode::shared_ptr & outScene)
{
    assert(scene && "Scene is NULL");
    assert(scene->mRootNode && "Scene->mRootNode is NULL");
    
    if (scene->HasCameras()) {
        for (int i = 0; i < scene->mNumCameras; ++i) {
            aiNode* cnd = scene->mRootNode->FindNode(scene->mCameras[i]->mName);
            if (cnd) {
            	cameraNodes.push_back(cnd);
            } else {
                throw std::runtime_error("Failed to find cameta node");
            }
        }
    } else {
        std::shared_ptr<Camera> camPtr = std::make_shared<Camera>();
        CameraNode::shared_ptr camNode = std::make_shared<CameraNode>(Camera::shared_ptr(NULL),
                                                                      "camera");
        SceneNode::shared_ptr outNode = std::static_pointer_cast<SceneNode>(outScene);
        outNode->add(camNode);
        camNode->setCamera(camPtr);
        camPtr->setPosition(Point3(0,0,-6));
        camPtr->setOrientationVector(Point3(0,1,0));
        camPtr->setTargetPoint(-Point3(0,0,0));
        float yfov = 45;
        yfov = (45) / 1.77;
        camPtr->setPerspectiveMatrix(yfov,
                                     1.33,
                                     0.01,
                                     1000);
    }
    if (scene->HasMeshes()) {
        aiNode * node = scene->mRootNode;
        SceneNode::shared_ptr rootNode = std::static_pointer_cast<SceneNode>(outScene);
        processNode(scene, node, rootNode);
    }
    
    {
        aiNode * node = scene->mRootNode;
        SceneNode::shared_ptr rootNode = std::static_pointer_cast<SceneNode>(outScene);
        processMaterial(scene, node, rootNode);
    }
}

void Scene::Impl::processNode(const aiScene * scene,
                              const aiNode *node,
                              SceneNode::shared_ptr & outNode)
{
    auto aT = node->mTransformation; aT.Transpose();
    Eigen::Affine3f t = Eigen::Affine3f(Eigen::Matrix4f(&aT.a1));
    outNode->transform() = outNode->transform() * t;
    outNode->setName(node->mName.C_Str());
    
    if (node->mNumMeshes > 0) {
        processMeshes(scene, node, outNode);
    }
    auto camIt = std::find(cameraNodes.begin(), cameraNodes.end(), node);
    if (camIt != cameraNodes.end()) {
        std::vector<aiNode*>::size_type ds = cameraNodes.end() - camIt - 1;
        processCamera(scene,
                      node,
                      scene->mCameras[ds],
                      outNode);
    }
    for (unsigned int i = 0; i < node->mNumChildren; ++i) {
        SceneNode::shared_ptr child = std::make_shared<SceneNode>();
        outNode->add(child);
        processNode(scene, node->mChildren[i], child);
    }
}

void Scene::Impl::processMeshes(const aiScene * scene,
                              	const aiNode *node,
                              	SceneNode::shared_ptr & outNode)
{

    for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
        SurfaceNode::shared_ptr surfaceNode = std::make_shared<SurfaceNode>(new Subdivision());
        outNode->add(surfaceNode);
        const aiMesh * mesh = scene->mMeshes[node->mMeshes[i]];
        surfaceNode->setName(mesh->mName.C_Str());
        // No faces, nothong to do...
        if (!mesh->HasFaces()) {
            continue;
        }
        std::vector<Vertex::size_t> map;
        Eigen::Vector3f p, n;
        if (mesh->HasPositions()) {
            for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
                p = Vector3(mesh->mVertices[i].x,
                            mesh->mVertices[i].y,
                            mesh->mVertices[i].z);
                auto viid = surfaceNode->surface()->addVertex(p);
                map.push_back(viid);
                
                if (mesh->HasNormals()) {
                    n = Vector3(mesh->mNormals[i].x,
                                mesh->mNormals[i].y,
                                mesh->mNormals[i].z
                                );
                    surfaceNode->surface()->vertex(viid)->normal() = n;
                }
                surfaceNode->surface()->vertex(viid)->color() = Color(1.0f, 1.0f, 1.0f);
            }
        }
        
        std::vector<Face::size_t> indices(3);
        for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
            indices[0] = map[mesh->mFaces[i].mIndices[0]];
            indices[1] = map[mesh->mFaces[i].mIndices[1]];
            indices[2] = map[mesh->mFaces[i].mIndices[2]];
            surfaceNode->surface()->addFace(indices);
        }

    }
}

void Scene::Impl::processCamera(const aiScene * scene,
                              	const aiNode *node,
                                const aiCamera *camera,
                              	SceneNode::shared_ptr & outNode)
{
    assert(scene != nullptr && node != nullptr && camera != nullptr);
    
    std::shared_ptr<Camera> camPtr = std::make_shared<Camera>();
    CameraNode::shared_ptr camNode = std::make_shared<CameraNode>(Camera::shared_ptr(NULL),
                                                                  camera->mName.C_Str());
    outNode->add(camNode);
    camNode->setCamera(camPtr);
    camPtr->setPosition(Point3(&camera->mPosition.x));
    camPtr->setOrientationVector(Point3(&camera->mUp.x));
    camPtr->setTargetPoint(Point3(&camera->mLookAt.x));
    float yfov = camera->mHorizontalFOV * 180 / M_PI;
    yfov = (camera->mHorizontalFOV * 2 * 180 / M_PI) / camera->mAspect;
    camPtr->setPerspectiveMatrix(yfov,
                                 camera->mAspect,
                                 camera->mClipPlaneNear,
                                 camera->mClipPlaneFar);
}

void Scene::Impl::processMaterial(const aiScene * scene,
                                  const aiNode * node,
                                  SceneNode::shared_ptr & outNode)
{
    std::shared_ptr<PhongMaterial> material = std::make_shared<PhongMaterial>();
    try {
        material->load();
        material->setDiffuse (Color(1.0f, 0.4f, 0.8f, 1.0f));
        material->setSpecular(Color(1.0f, 1.0f, 1.0f, 1.0f));
        material->setAmbient (Color(0.1f, 0.1f, 0.1f, 1.0f));
        material->setExponent(200);
        
        auto it = outNode->treeIterator();
        while (it.hasNext()) {
            auto node = it.next();
            std::cout << "Node: " << node->name() << " Type: " << node->nodeType() << "\n";
            if (node->nodeType() == NT_Surface) {
                SurfaceNode::shared_ptr surface = std::static_pointer_cast<SurfaceNode>(node);
                surface->setMaterial(material);
            }
        }
    } catch(core::GlException & e) {
        std::cerr   << "GLException: " << e.what() << std::endl
        << e.error() << ": " << e.errorString() << std::endl;
    } catch (std::exception & e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    
}


static void
getCameraRecursive(const SceneNode::const_shared_ptr & scene,
                   CameraNode::shared_ptr & container)
{
    if (container) return;
    
    auto it = scene->constIterator();
    while(it.hasNext()) {
        SceneNode::shared_ptr child = it.next();
        CameraNode::shared_ptr cam = std::dynamic_pointer_cast<CameraNode>(child);
        if (cam) {
            container = cam;
        } else {
            getCameraRecursive(child, container);
        }
    }
}

CameraNode::shared_ptr
Scene::getCamera() const
{
    CameraNode::shared_ptr res;
    
    SceneNode::const_shared_ptr node = std::dynamic_pointer_cast<const SceneNode>(this->shared_from_this());
    getCameraRecursive(node, res);
    
    return res;
}
