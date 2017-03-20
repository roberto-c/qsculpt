//
//  ForceFunctors.h
//  PlastilinaCore
//
//  Created by Juan Roberto Cabral Flores on 12/29/13.
//
//

#ifndef PlastilinaCore_ForceFunctors_h
#define PlastilinaCore_ForceFunctors_h

#include <functional>

namespace physics
{
class SimSystem;
class Actor;

typedef std::function<Eigen::Vector3f(SimSystem*, Actor*)> ForceFunction;

struct Gravity
{
    Eigen::Vector3f operator()(SimSystem*, Actor*)
    {
        Eigen::Vector3f r(0, -.98f, 0);
        return r;
    }
};

struct Rejection
{
    Eigen::Vector3f operator()(SimSystem* s, Actor* a);
};
}

#endif
