#pragma once
#ifndef __PLASTILINACORE_PHYSICS_ACTOR_H__
#define __PLASTILINACORE_PHYSICS_ACTOR_H__

namespace physics {
    class Actor {
        public:
        
        typedef std::shared_ptr<Actor> shared_ptr;
        typedef std::weak_ptr<Actor> weak_ptr;
        
        
        Eigen::Vector3f x;
        Eigen::Vector3f v;
        Eigen::Vector3f f;
        float 			invMass;
    };
};

#endif
