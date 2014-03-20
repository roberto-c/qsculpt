/***************************************************************************
 *   Copyright (C) 2014 by Juan R Cabral                                   *
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
#include <PlastilinaCore/Stable.h>
#include <PlastilinaCore/physics/SimSystem.h>
#include <PlastilinaCore/physics/Actor.h>
#include <vector>
#include <map>
#include <algorithm>
#include <functional>

namespace physics {
    typedef Eigen::Hyperplane<float, 3> Plane3f;
    
    struct SimSystem::Impl {
        std::vector<Actor::shared_ptr> 			actorList;
        std::map<std::string, ForceFunction>	forceList;
        std::vector<Plane3f>					wallList;
    };
    
    SimSystem::SimSystem() : d(new Impl())
    {
        
    }
    
    SimSystem::~SimSystem() {
        
    }
    
    void SimSystem::step(float step)
    {
//        Eigen::Vector3f floorPoint(0,-0.5f,0);
//        Eigen::Hyperplane<float, 3> floor(Eigen::Vector3f(0,1,0).normalized(), floorPoint);
        
        // clear forces;
        for(auto a : d->actorList) {
            a->f.setZero();
        }
        // Compute forces
        for(auto a : d->actorList) {
            for(auto f : d->forceList) {
                a->f += f.second(this, a.get());
            }
        }
        // update positions
        for(auto a : d->actorList) {
            a->x0 = a->x;
            a->x += a->v * step;
            a->v += a->f * a->invMass * step;
        }
        // Check for collisions
        for(auto a : d->actorList) {
            for (auto plane : d->wallList) {
                Eigen::Vector3f planePoint = Eigen::Vector3f(0,-0.5f,0); //plane.projection(Eigen::Vector3f(0,0,0));
                if (plane.normal().dot(a->x - planePoint) < 0) {
                    // 2(N.L)N - L
                    float l = a->v.norm();
                    Eigen::Vector3f r = 2 * (plane.normal().dot(a->v.normalized()))*plane.normal() - a->v.normalized();
                    a->x = a->x0;
                    a->v = -r*(l*0.8f);

                }
            }
        }
    }
    
    void SimSystem::addActor(Actor::shared_ptr & a)
    {
        d->actorList.push_back(a);
    }
    
    void SimSystem::removeActor(Actor::shared_ptr & a)
    {
        auto e = std::remove(d->actorList.begin(), d->actorList.end(), a);
        if (e == d->actorList.end()) {
            std::cerr << "WARN: removeActor: object not found." << std::endl;
        }
    }
    
    void SimSystem::clearActors()
    {
        d->actorList.clear();
    }
    
    std::vector<Actor::shared_ptr> & SimSystem::actors()
    {
        return d->actorList;
    }
    
    const std::vector<Actor::shared_ptr> & SimSystem::actors() const
    {
        return d->actorList;
    }
    
    void SimSystem::addForceFunctor(const std::string & key, ForceFunction f)
    {
        d->forceList.insert(std::make_pair(key,f));
    }
    
    void SimSystem::removeForceFunctor(const std::string & key)
    {
        d->forceList.erase(key);
    }
    
    void SimSystem::clearForceFunctors()
    {
        d->forceList.clear();
    }
    
    void SimSystem::addWall(Eigen::Vector3f n, Eigen::Vector3f p)
    {
        d->wallList.push_back(Plane3f(n,p));
    }
    
    void SimSystem::clearWalls()
    {
        d->wallList.clear();
    }
}
