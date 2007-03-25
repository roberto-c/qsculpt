/*
 *  voxelbox.h
 *  solidpaint3d
 *
 *  Created by Roberto Cabral on 2/19/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef VOXELBOX_H
#define VOXELBOX_H

#include "voxelobject.h"

/**
Draw a 3D box.
 
	@author Juan Roberto Cabral Flores <roberto.cabral@gmail.com>
 */
class VoxelBox : public VoxelObject
{
public:
    VoxelBox();
	
    virtual ~VoxelBox();
    
protected:
		/**
		* Initializes the points vector.
		 */
		virtual void initPoints();
    
private:
		Point3D m_vertex[8];
	
};

#endif
