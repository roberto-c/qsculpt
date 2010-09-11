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
#ifndef CONFIGCONTAINER_H
#define CONFIGCONTAINER_H

#include "IConfigContainer.h"
#include <QMap>
#include <QString>
#include <QVariant>
#include "Point3D.h"

/**
 * Implementation of IConfigContainer interface
 *
 * @author Juan Roberto Cabral Flores <roberto.cabral@gmail.com>
 */
class ConfigContainer : public IConfigContainer
{
public:
    ConfigContainer();

    ConfigContainer(const ConfigContainer& cpy);

    virtual ~ConfigContainer();

    /*
     * IConfigContainer interface
     */
    virtual IConfigContainer* clone() const;
    virtual bool containsKey(QString key);
    virtual double getDouble(QString key);
    virtual int getInt(QString key);
    virtual QString getString(QString key);
    virtual void setDouble(QString key, double value);
    virtual void setInt(QString key, int value);
    virtual void setString(QString key, QString value);
    virtual bool getBool(QString key);
    virtual void setBool(QString key, bool value);
    virtual Point3 getPoint3D(QString key);
    virtual void setPoint3D(QString key, const Point3 &v);
    
private:
    QMap<QString, QVariant> m_container;
};

#endif

