/**
 *
 */
#pragma once
#ifndef __PLASTILINACORE_PHYSICS_SIMSYSTEM_H__
#define __PLASTILINACORE_PHYSICS_SIMSYSTEM_H__

#include <PlastilinaCore/physics/Actor.h>
#include <functional>

namespace physics {
    class Actor;
    class Particle;
    class Box;
    class Cilinder;
    class Sphere;
    class Mesh;
    class SimSystem;
    
    typedef std::function<Eigen::Vector3f (SimSystem *, Actor *) > ForceFunction;
    
    /**
     * \class SimSystem
     * \brief Similation system class. This class is the core of the physics 
     * simulation engine.
     */
    class SimSystem {
        public:
        
        SimSystem();
        ~SimSystem();
        
        void step(float step);
        
        void addActor(Actor::shared_ptr & a);
        void removeActor(Actor::shared_ptr & a);
        void clearActors();
        
        std::vector<Actor::shared_ptr> & actors();
        const std::vector<Actor::shared_ptr> & actors() const;
        
        void addForceFunctor(const std::string & key, ForceFunction f);
        void removeForceFunctor(const std::string & key);
        void clearForceFunctors();
        
        private:
        struct Impl;
        std::unique_ptr<Impl> d;
    };
    
    
}
#endif
