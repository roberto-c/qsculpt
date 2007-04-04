/***************************************************************************
*   Copyright (C) 2007 by Juan Roberto Cabral Flores   *
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
