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
#include <PlastilinaCore/Camera.h>
#include <PlastilinaCore/IRenderer.h>
#include <PlastilinaCore/Scene.h>
#include <PlastilinaCore/SceneNode.h>
#include <PlastilinaCore/geometry/Ray.h>
#include <PlastilinaCore/geometry/Aabb.h>
#include <PlastilinaCore/Octree.h>

//namespace  {
    struct CenterMassFn {
        Vector3 operator()(SceneNode::weak_ptr) {
            return Vector3();
        }
    };
//};

struct Scene::Impl {
    //data::Octree<SceneNode::weak_ptr,CenterMassFn> octree;
    
    Impl()
    {
    }
    
    SceneNode::shared_ptr findByIidRecursive(SceneNode::const_shared_ptr root,
                                             uint IID) const;
	
	void renderRecursive(RenderState & state, const SceneNode * root) const;
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
		state.camera = getCamera()->camera().get();
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
