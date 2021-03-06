/***************************************************************************
 *   Copyright (C) 2010 by Juan Roberto Cabral Flores                      *
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
#ifndef QUAD_H
#define QUAD_H

#include <PlastilinaCore/subdivision/Subdivision.h>

/**
 * Draw a 3D box.
 *
 * @author Juan Roberto Cabral Flores <roberto.cabral@gmail.com>
 */
class Quad : public Subdivision
{
  public:
    Quad();

    virtual ~Quad();

  protected:
    /**
     * Initializes the points vector.
     */
    virtual void initPoints();

    /**
     * Initialized the triangles used on the object.
     */
    virtual void initTriangles();

  private:
    Point3 m_vertex[4];
};

#endif // QUAD_H
