/**
 *
 */
#pragma once
#ifndef __PLASTILINACORE_PHYSICS_SIMSYSTEM_H__
#define __PLASTILINACORE_PHYSICS_SIMSYSTEM_H__

#include <PlastilinaCore/physics/Actor.h>
#include <PlastilinaCore/physics/ForceFunctors.h>

namespace physics
{
/**
 * \class SimSystem
 * \brief Similation system class. This class is the core of the physics
 * simulation engine.
 *
 * The simulation acts over all the actors added to simulation system.
 */
class SimSystem
{
  public:
    /**
     * Constructs a simulation system.
     */
    SimSystem();
    ~SimSystem();

    /**
     * Advance the simulation step by step time units.
     *
     * @param step delta time used to step into the simulation.
     */
    void step(float step);

    /**
     * Add a new actor into the system.
     *
     * @param a Actor to add into the system.
     */
    void addActor(Actor::shared_ptr& a);

    /**
     * Remove an actor from the simulation system.
     *
     * @param a Actor to remove from the system.
     */
    void removeActor(Actor::shared_ptr& a);

    /**
     * Remove all actors from the simulation system.
     */
    void clearActors();

    /**
     * Returns the list of actors in the system simulation
     *
     * @returns vector with all actors in the system
     */
    std::vector<Actor::shared_ptr>&       actors();
    const std::vector<Actor::shared_ptr>& actors() const;

    /**
     * Adds a functor used to compute the force applied to each actor.
     * The functor has the signature as:
     * void forcefn(physics::SimSystem*, physics::Actor*);
     *
     * The functor is associated to a key used to identify the function. This
     * helps to register or add the same function multiple times.
     *
     * @param key a name used as key to identify the functor object.
     * @param f reference to a function pointer or a functor object.
     */
    void addForceFunctor(const std::string& key, ForceFunction f);
    void removeForceFunctor(const std::string& key);
    void clearForceFunctors();

    /**
     * Adds a new wall or floor to the system to enclose the particles in.
     */
    void addWall(Eigen::Vector3f n, Eigen::Vector3f p);
    void clearWalls();

  private:
    struct Impl;
    std::unique_ptr<Impl> d;
};
}
#endif
