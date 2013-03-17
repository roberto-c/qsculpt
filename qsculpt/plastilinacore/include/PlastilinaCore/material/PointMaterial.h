//
//  PhongMaterial.h
//  TestApp
//
//  Created by Juan Roberto Cabral Flores on 8/5/12.
//  Copyright (c) 2012 Juan Roberto Cabral Flores. All rights reserved.
//

#ifndef __TestApp__PointMaterial__
#define __TestApp__PointMaterial__

#include <iostream>

#include <PlastilinaCore/Material.h>
#include <PlastilinaCore/Color.h>

class PointMaterial : public Material
{
public:
    PointMaterial();
    
    virtual ~PointMaterial();
    
    virtual void load();
    
    virtual void unload();
    
    virtual void setup(const std::shared_ptr<SceneNode> & doc);
	
	virtual void setup(const std::shared_ptr<const SceneNode> & doc);
    
private:
    struct Impl;
    std::unique_ptr<Impl> d;
};

#endif /* defined(__TestApp__PhongMaterial__) */
