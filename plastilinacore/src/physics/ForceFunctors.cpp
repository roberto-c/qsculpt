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
#include <PlastilinaCore/physics/ForceFunctors.h>
#include <PlastilinaCore/physics/SimSystem.h>
#include <PlastilinaCore/physics/Actor.h>

namespace physics {

    Eigen::Vector3f Rejection::operator()(SimSystem * s, Actor * a) {
        Eigen::Vector3f r(0,0,0);
        for(auto e : s->actors()) {
            if (a == e.get()) continue;
            
            if (e->type == ActorType::Sphere && a->type == ActorType::Sphere) {
                auto sphere1 = std::static_pointer_cast<physics::Sphere>(e);
                auto sphere2 = static_cast<physics::Sphere*>(a);
                Eigen::Vector3f f = (sphere1->x - sphere2->x);
                if (f.norm() < sphere2->radius + sphere1->radius) {
                    Eigen::Vector3f n = f.normalized();
                    Eigen::Vector3f v = sphere1->v + sphere2->v;
                    float l = a->v.norm();
                    f = 2 * (n.dot(v.normalized()))*n - v.normalized();
                    f *= -(l*0.8f);
                    r = f;
                }
            }
        }
        return r;
    }

}

