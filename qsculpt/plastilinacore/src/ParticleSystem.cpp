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
#include <PlastilinaCore/ParticleSystem.h>
#include <PlastilinaCore/Particle.h>
#include <PlastilinaCore/IIntegrator.h>


#define MAX_NUM_PARTICLES (100)

namespace physics {
    typedef std::vector<Eigen::Vector4f,Eigen::aligned_allocator<Eigen::Vector4f> > VVector4;
    
    struct ParticleSystem::Impl {
        std::unique_ptr<IIntegrator> _int;
        std::vector<Particle> particles; 
        VVector4  dp;
        VVector4  dv;
        
        void step(double time) {
            size_t size = particles.size();
            
            // clear forces / set force to gravity
            for (size_t i = 0; i < size; ++i) {
                particles[i].f[0] = 0;
                particles[i].f[1] = -0.3;
                particles[i].f[2] = 0;
                particles[i].f[3] = 0;
            }
            
            // derive pos and vel
            for (size_t i = 0; i < size; ++i) {
                dp[i] = particles[i].v;
                dv[i] = particles[i].f * particles[i].invm;
            }
            
            // scale
            for (size_t i = 0; i < size; ++i) {
                dp[i] *= time;
                dv[i] *= time;
            }
            
            // update position
            for (size_t i = 0; i < size; ++i) {
                particles[i].x += dp[i];
                particles[i].v += dv[i];
            }
            computeForce();
        }
        
        void derive(VVector4 &, VVector4 &) {
        }
        
        void computeForce() {
        }
        
    };
    
    ParticleSystem::ParticleSystem() : _d(new Impl) {
        _d->_int.reset( new EulerIntegrator);
        _d->particles.reserve(MAX_NUM_PARTICLES);
        _d->dp.reserve(MAX_NUM_PARTICLES);
        _d->dv.reserve(MAX_NUM_PARTICLES);
    }
    
    void ParticleSystem::addParticle(Particle *p) {
        if (p) _d->particles.push_back(*p);
    }
    
    void ParticleSystem::step(double time) {
        _d->step(time);
    }
    
    void ParticleSystem::setResetFunctor(utils::Functor ) {
        //_d->setResetFunctor(fn);
    }
};

