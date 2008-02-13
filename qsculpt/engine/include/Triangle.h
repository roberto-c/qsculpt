/***************************************************************************
*   Copyright (C) 2006 by Juan Roberto Cabral Flores   *
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
#ifndef TRIANGLE_H
#define TRIANGLE_H

/**
 * @class Object3D
 */
class Object3D
{
    protected:
    /**
     * Triangle class. This class conatains references to points that should
     * form a triangle.
     *
     * The points are indices of the Object3D point list. So, this class It's
     * only meant to be used with the Object3D class (its not a generic triangle
     * class).
     *
     * @author Juan Roberto Cabral Flores <roberto.cabral@gmail.com>
     */
    class Triangle
    {
    public:
        /**
         * Construct a new triangle. The triangle is composed by the passed
         * points ids or references.
         *
         * @param p1 first point that form the triangle.
         * @param p2 second point that form the triangle.
         * @param p3 third point that form the triangle.
         */
        Triangle(unsigned int p1, unsigned int p2, unsigned int p3)
        { m_p[0] = p1; m_p[1] = p2; m_p[2] = p3;}

        /**
         * Class destructor.
         */
        ~Triangle(){}

        /**
         * Sets the first point index reference.
         *
         * @param p index of the point.
         */
        void setPoint1(unsigned int p) { m_p[0] = p;}
        
        /**
         * Gets the first point index reference.
         *
         * @return the point index
         */
        unsigned int getPoint1() { return m_p[0]; }
        
        /**
         * Sets the second point index reference.
         *
         * @param p index of the point.
         */
        void setPoint2(unsigned int p) { m_p[1] = p;}
        
        /**
         * Gets the second point index reference.
         *
         * @return the point index
         */
        unsigned int getPoint2() { return m_p[1]; }
        
        /**
         * Sets the third point index reference.
         *
         * @param p index of the point.
         */
        void setPoint3(unsigned int p) { m_p[2] = p;}
        
        /**
         * Gets the third point index reference.
         *
         * @return the point index
         */
        unsigned int getPoint3() { return m_p[2]; }
        
        /**
         * Gets all the indices as an array. The array contains 3 elements (one
         * element by point in the triangle).
         * 
         * @return points indices array.
         */
        unsigned int& operator[](int index) {
        	 return (index < 3 && index >= 0) ? m_p[index] : NULL;
		}
        
    private:
        unsigned int m_p[3]; /**< array of indices that form the triangle */
    };
};
#endif
