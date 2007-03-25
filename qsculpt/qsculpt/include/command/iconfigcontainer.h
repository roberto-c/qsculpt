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
#ifndef ICONFIGCONTAINER_H
#define ICONFIGCONTAINER_H

#include <QString>
#include "point3d.h"

/**
 * Container for configuration parameters of the commands.
 * 
 * @author Juan Roberto Cabral Flores <roberto.cabral@gmail.com>
*/
class IConfigContainer{
public:
    /**
     * Default constructor.
     */
    IConfigContainer(){}

    /**
     * Default destructor.
     */
    virtual ~IConfigContainer(){}

    /**
     * Retrieves true if a config parameter key exist.
     *
     * @param key key name of the parameter to retrieve.
     *
     * @return true if the key exists. Otherwise, false.
     */
    virtual bool containsKey(QString key) = 0;
    
    /**
     * Get a config string value. The key is the name of the parameter to
     * retrieve. Return an empty string if the key is not found.
     *
     * @param key key name of the parameter to retrieve.
     *
     * @return config value
     */
    virtual QString getString(QString key) = 0;
    
    /**
     * Set a config string value. The key is the name of the parameter to
     * set. If the key is not found, then the key is added.
     *
     * @param key key name of the parameter to set.
     * @param value config value to set.
     *
     */
    virtual void setString(QString key, QString value) = 0;
    
    /**
     * Get a config int value. The key is the name of the parameter to
     * retrieve. Return a default value 0 in case that the key isn't found.
     *
     * @param key key name of the parameter to retrieve.
     *
     * @return config value
     */
    virtual int getInt(QString key) = 0;
    
    /**
     * Set a config int value. The key is the name of the parameter to
     * set. If the key is not found, then the key is added.
     *
     * @param key key name of the parameter to set.
     * @param value config value to set.
     *
     */
    virtual void setInt(QString key, int value) = 0;
    
    /**
     * Get a config double value. The key is the name of the parameter to
     * retrieve. Return a default value 0 in case that the key isn't found.
     *
     * @param key key name of the parameter to retrieve.
     *
     * @return config value
     */
    virtual double getDouble(QString key) = 0;
    
    /**
     * Set a config double value. The key is the name of the parameter to
     * set. If the key is not found, then the key is added.
     *
     * @param key key name of the parameter to set.
     * @param value config value to set.
     *
     */
    virtual void setDouble(QString key, double value) = 0;
    
    /**
     * Get a bool config value. The key is the name of the parameter to
     * retrieve. Return a default value of false in case that the key isn't found.
     *
     * @param key key name of the parameter to retrieve.
     *
     * @return config value
     */
    virtual bool getBool(QString key) = 0;
    
    /**
     * Set a config boolean value. The key is the name of the parameter to
     * set. If the key is not found, then the key is added.
     *
     * @param key key name of the parameter to set.
     * @param value config value to set.
     *
     */
    virtual void setBool(QString key, bool value) = 0;
    
    virtual Point3D getPoint3D(QString key)=0;
    
    
    virtual void setPoint3D(QString key, const Point3D &v) = 0;
};

#endif
