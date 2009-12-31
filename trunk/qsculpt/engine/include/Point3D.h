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
#ifndef POINT3D_H
#define POINT3D_H

#include <qstring.h>
#include <QMetaType>
#include <QHash>
#include <math.h>
#include <limits>

/**
 * Represents a point in 3D space.
 *
 * @author Juan Roberto Cabral Flores <roberto.cabral@gmail.com>
 */
template<class T>
class Point3D_t{
public:
    /**
     * Returns a invalid Point3D_t. Serves for the purpose of comparations.
     */
    static const Point3D_t<T>& null()
    {
        static Point3D_t<T> null = Point3D_t<T>(std::numeric_limits<T>::quiet_NaN(),
                                            std::numeric_limits<T>::quiet_NaN(),
                                            std::numeric_limits<T>::quiet_NaN());
        return null;
    }

    /**
     * Default contructor. Initializes all components to 0.
     */
    Point3D_t() {
        m_data[0] = m_data[1] = m_data[2] = 0;
    }
    
    /**
     * Contructor with initial values.
     *
     * @param x x component initial value
     * @param y y component initial value
     * @param z z component initial value
     */
    Point3D_t(T x, T y, T z) {
        m_data[0] = x;
        m_data[1] = y;
        m_data[2] = z;
    }

    
    Point3D_t(const Point3D_t<T> &cpy) {
        m_data[0] = cpy.m_data[0];
        m_data[1] = cpy.m_data[1];
        m_data[2] = cpy.m_data[2];
    }
        
    /**
     * Get the string representation of the point.
     * This function ouputs a string with the representation of this point
     * as coordinates. Example:
     * 
     * Point3D point(2, 3, 5.25);<br>
     * std::cout << "Point: " << point.toString().toAscii() << std::endl;
     *
     * gives the following ouput:<br>
     *
     * Point: (2. 3. 5.25)
     *
     * @return a string with the point coordenates.
     */
    QString toString() const {
        return QString("(" + QString::number(m_data[0]) + ", " +
                QString::number(m_data[1]) + ", " +
                QString::number(m_data[2]) + ")");
    }
    
    /**
     * Set the x component value.
     *
     * @param value x component value
     */
    void setX(T value) { m_data[0] = value;};
    
    /**
     * Get the x component.
     *
     * @return x component of the point.
     */
    T getX() const { 
        return m_data[0]; 
    };
    
    /**
     * Set the y component value.
     *
     * @param value y component value
     */
    void setY(T value) { 
        m_data[1] = value;
    };
    
    /**
     * Get the y component.
     *
     * @return y component of the point.
     */
    T getY() const { 
        return m_data[1]; 
    };
    
    /**
     * Set the z component value.
     *
     * @param value z component value
     */
    void setZ(T value) { 
        m_data[2] = value;
    };
    
    /**
     * Get the z component.
     *
     * @return z component of the point.
     */
    T getZ() const { 
        return m_data[2]; 
    };
    
    /**
     * Get each componet of the point.
     *
     * @param x return the x component in this param. 
     * @param y return the y component in this param. 
     * @param z return the z component in this param. 
     */
    void getPoint(T &x, T &y, T &z) const{
        x = m_data[0];
        y = m_data[1];
        z = m_data[2];
    }
    
    /**
     * Set each componet of the point.
     *
     * @param x x component of the point. 
     * @param y y component of the point. 
     * @param z z component of the point. 
     */
    void setPoint(T x, T y, T z) {
        m_data[0] = x;
        m_data[1] = y;
        m_data[2] = z;
    }
    
    /**
     * Return the point's components as an array.
     */
    const T* getPoint() const {
        return m_data;
    }
    
    /**
     * Normalize the vector.
     */
    Point3D_t<T>& normalize() {
        T len = length();
        m_data[0] /= len;
        m_data[1] /= len;
        m_data[2] /= len;
    
        return *this;
    }
    
    /**
     * get the length of the vector.
     */
    T length() {
        return sqrt(m_data[0]*m_data[0] + m_data[1]*m_data[1] + m_data[2]*m_data[2]);
    }
    
    /**
     * 
     */
    inline T dotProduct(const Point3D_t<T> &v);
    
    /**
     * 
     */
    inline Point3D_t<T> crossProduct(const Point3D_t<T> &v);
    
    /**
     * operator== oveload.
     *
     * @param m point to equals to.
     */
    bool operator==(const Point3D_t<T>& value) const {
        if (this == &value)
            return true;
        
        return (m_data[0] == value.m_data[0])
                && (m_data[1] == value.m_data[1])
                && (m_data[2] == value.m_data[2]);
    }
    
    /**
     * operator= oveload.
     *
     * @param m point to equals to.
     */
    bool operator!=(const Point3D_t<T>& value) {       
        return !operator==(value);
    }
    
    /**
     * operator= oveload.
     *
     * @param m point to equals to.
     */
    Point3D_t<T>& operator=(const Point3D_t<T>& value) {
        if (this == &value)
            return *this;
        
        m_data[0] = value.m_data[0];
        m_data[1] = value.m_data[1];
        m_data[2] = value.m_data[2];

        return *this;
    }
    
	/**
     * operator[] oveload.
     *
     * @param index component index to retrieve.
     */
    T& operator[](int index) {
        return m_data[index];
    }
	
	/**
     * operator[] oveload.
     *
     * @param index component index to retrieve.
     */
    const T& operator[](int index) const {
        return m_data[index];
    }
	
	
    /**
     * operator+ oveload.
     *
     * @param m point to add.
     */
    Point3D_t<T> operator+(const Point3D_t<T>& value) const{
        Point3D_t<T> tmp;
        tmp.m_data[0] = m_data[0] + value.m_data[0];
        tmp.m_data[1] = m_data[1] + value.m_data[1];
        tmp.m_data[2] = m_data[2] + value.m_data[2];

        return tmp;
    }
    
    /**
     * operator- oveload.
     *
     * @param m point to rest.
     */
    Point3D_t<T> operator-(const Point3D_t<T>& value) const {
        Point3D_t<T> tmp;
        tmp.m_data[0] = m_data[0] - value.m_data[0];
        tmp.m_data[1] = m_data[1] - value.m_data[1];
        tmp.m_data[2] = m_data[2] - value.m_data[2];

        return tmp;
    }
    
    template<class F>
    friend inline const Point3D_t<F> operator*( const Point3D_t<F> &val, F c);
    
    template<class F>
    friend inline const Point3D_t<F> operator*( F c, const Point3D_t<F> &val);
    
    template<class F>
    friend inline const Point3D_t<F> operator/( const Point3D_t<F> &val, F c);
    
    template<class F>
    friend inline const Point3D_t<F> operator/( F c, const Point3D_t<F> &val);

private:
    T m_data[3];   /**< Holds components data. [0] is X, [1] is Y and [2] is Z*/
};

template<class T>
inline T Point3D_t<T>::dotProduct(const Point3D_t<T> &v)
{
    return m_data[0]*v.m_data[0] + m_data[1]*v.m_data[1] + m_data[2]*v.m_data[2];
}

template<class T>
inline Point3D_t<T> Point3D_t<T>::crossProduct(const Point3D_t<T> &v)
{
    return Point3D_t<T>( m_data[1]*v.m_data[2] - m_data[2]*v.m_data[1],
                     m_data[2]*v.m_data[0] - m_data[0]*v.m_data[2],
                     m_data[0]*v.m_data[1] - m_data[1]*v.m_data[0]);
}

template<class F>
inline const Point3D_t<F> operator*( const Point3D_t<F> &val, F c)
{
    return Point3D_t<F>(val.m_data[0] * c,
                   val.m_data[1] * c,
                   val.m_data[2] * c);
}

template<class F>
inline const Point3D_t<F> operator*( F c, const Point3D_t<F> &val)
{
    return Point3D_t<F>(val.m_data[0] * c,
                   val.m_data[1] * c,
                   val.m_data[2] * c);
}

template<class F>
inline const Point3D_t<F> operator/( const Point3D_t<F> &val, F c)
{ 
    return Point3D_t<F>(val.m_data[0] / c,
                   val.m_data[1] / c,
                   val.m_data[2] / c);
}

template<class F>
inline const Point3D_t<F> operator/( F c, const Point3D_t<F> &val)
{
    return Point3D_t<F>(val.m_data[0] / c,
                   val.m_data[1] / c,
                   val.m_data[2] / c);
}

//typedef Point3D_t<float> Point3D;
//typedef Point3D_t<float> Vertex;
//typedef Point3D_t<float> Normal;
//typedef Point3D_t<float> Vector3;
typedef Eigen::Vector3f Point3D;
typedef Eigen::Vector3f Vector3;
typedef Eigen::Vector3f Normal;

Q_DECLARE_METATYPE(Point3D)

#endif

