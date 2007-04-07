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
#include "configcontainer.h"

ConfigContainer::ConfigContainer(): IConfigContainer()
{
}


ConfigContainer::~ConfigContainer()
{
}


bool ConfigContainer::containsKey(QString key)
{
    return m_container.contains(key);
}

double ConfigContainer::getDouble(QString key)
{
    return !m_container.contains(key) ? 0.0 : m_container[key].toDouble();
}

int ConfigContainer::getInt(QString key)
{
    return !m_container.contains(key) ? 0 : m_container[key].toInt();
}

QString ConfigContainer::getString(QString key)
{
    return !m_container.contains(key) ? QString("") : m_container[key].toString();
}

void ConfigContainer::setDouble(QString key, double value)
{
    m_container[key] = QVariant(value);
}

void ConfigContainer::setInt(QString key, int value)
{
    m_container[key] = QVariant(value);
}

void ConfigContainer::setString(QString key, QString value)
{
    m_container[key] = QVariant(value);
}

bool ConfigContainer::getBool(QString key)
{
    return !m_container.contains(key) ? false : m_container[key].toBool();
}

void ConfigContainer::setBool(QString key, bool value)
{
    m_container[key] = QVariant(value);
}

Point3D ConfigContainer::getPoint3D(QString key)
{
    return !m_container.contains(key) ? Point3D( 0, 0, 0) : m_container[key].value<Point3D>();
}

void ConfigContainer::setPoint3D(QString key, const Point3D &v)
{
    m_container[key] = QVariant::fromValue(v);
}

