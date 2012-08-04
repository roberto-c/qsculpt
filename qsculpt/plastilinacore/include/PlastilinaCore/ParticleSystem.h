/***************************************************************************
 *   Copyright (C) 2011 by Juan Roberto Cabral Flores   *
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

#ifndef PARTICLESYSTEM_H_
#define PARTICLESYSTEM_H_

#include <PlastilinaCore/Utilities.h>

namespace physics {
    class Particle;
    
    class ParticleSystem {
        struct Impl;
        std::unique_ptr<Impl> _d;
        
    public:
        enum IntegratorType {
            Euler
        };
        
        ParticleSystem();
        
        /**
         * 
         */
        void addParticle(Particle *p);
        
        /**
         *
         */
        void removeParticle(Particle *p);
        
        /**
         * Set the integrator used in the particle system
         */
        void setIntegratorType(IntegratorType type);
        
        /**
         * Returns the integrator used in the particle system
         */
        IntegratorType integrator();
        
        /**
         * 
         */
        void reset();
        
        /**
         * Calculate the next step in the simulation
         */
        void step(double time);
        
        /**
         * Set a function object to use to reset the simulation system
         */
        void setResetFunctor(utils::Functor fn);
    };
};

#endif
