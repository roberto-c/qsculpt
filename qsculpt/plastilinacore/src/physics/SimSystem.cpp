/**
 *
 */
#include <PlastilinaCore/Stable.h>
#include <PlastilinaCore/physics/SimSystem.h>
#include <PlastilinaCore/physics/Actor.h>
#include <vector>
#include <map>
#include <algorithm>
#include <functional>

namespace physics {
    
    struct SimSystem::Impl {
        std::vector<Actor::shared_ptr> 			actorList;
        std::map<std::string, ForceFunction>	forceList;
    };
    
    SimSystem::SimSystem() : d(new Impl())
    {
        
    }
    
    void SimSystem::step(float step)
    {
        // clear forces;
        for(auto e : d->actorList) {
            e->f.setZero();
        }
        for(auto a : d->actorList) {
            for(auto f : d->forceList) {
                a->f += f.second(this, a.get());
            }
        }
        // update positions
        for(auto e : d->actorList) {
            e->x += e->v * step;
            e->v += e->f * e->invMass * step;
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
}
