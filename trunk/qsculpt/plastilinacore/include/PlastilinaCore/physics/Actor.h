#pragma once
#ifndef __PLASTILINACORE_PHYSICS_ACTOR_H__
#define __PLASTILINACORE_PHYSICS_ACTOR_H__

namespace physics {
    enum class ActorType {
        Null,
        Particle,
        Plane,
        Box,
        Sphere,
    };
    
    class Plane;
    class Sphere;
    
    class Actor {
        public:
        
        typedef std::shared_ptr<Actor> shared_ptr;
        typedef std::weak_ptr<Actor> weak_ptr;
        
        ActorType 		type;
        Eigen::Vector3f x;
        Eigen::Vector3f x0;
        Eigen::Vector3f v;
        Eigen::Vector3f f;
        float 			invMass;
        bool			fixed;
        
        Actor() : type(ActorType::Null), fixed(false) {}
        Actor(ActorType type) : type(type), fixed(false) {}
        
        bool intersects(Actor * a);
        bool intersects(Sphere * a);
        bool intersects(Plane * a);
    };
    
    class Particle : public Actor {
    public:
        typedef std::shared_ptr<Particle> shared_ptr;
        typedef std::weak_ptr<Particle> weak_ptr;
        
        Particle() : Actor(ActorType::Particle) {}
    };
    
    class Plane : public Actor {
    public:
        Eigen::Hyperplane<float, 3> plane;
        
        Plane() : Actor(ActorType::Plane){
        }
    };
    
    class Sphere : public Actor {
    public:
        typedef std::shared_ptr<Sphere> shared_ptr;
        typedef std::weak_ptr<Sphere> weak_ptr;
        
        float 			radius;
        
        Sphere() : Actor(ActorType::Sphere),radius(1.0f) {}
    };
    
    struct IntersectionRec {
        Eigen::Vector3f p;
        float 			t;
    };
    bool intersects(Actor::shared_ptr a1, Actor::shared_ptr a2, std::vector<IntersectionRec> * recs);
    
    bool intersects(Plane::shared_ptr a1, Particle::shared_ptr a2, std::vector<IntersectionRec> * recs);
    bool intersects(Plane::shared_ptr a1, Plane::shared_ptr a2, std::vector<IntersectionRec> * recs);
    bool intersects(Plane::shared_ptr a1, Sphere::shared_ptr a2, std::vector<IntersectionRec> * recs);
    bool intersects(Sphere::shared_ptr a1, Sphere::shared_ptr a2, std::vector<IntersectionRec> * recs);
    
};

#endif
