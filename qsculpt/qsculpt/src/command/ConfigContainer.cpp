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
#include "Stable.h"
#include "command/ConfigContainer.h"

ConfigContainer::ConfigContainer(): IConfigContainer()
{
}

ConfigContainer::ConfigContainer(const ConfigContainer& cpy)
: IConfigContainer(cpy),
m_container(cpy.m_container)
{
}

ConfigContainer::~ConfigContainer()
{
}

IConfigContainer* ConfigContainer::clone() const
{
    return new ConfigContainer(*this);
}

bool ConfigContainer::containsKey(const QString& key)
{
    return m_container.contains(key);
}

double ConfigContainer::getDouble(const QString& key)
{
    return !m_container.contains(key) ? 0.0 : m_container[key].toDouble();
}

int ConfigContainer::getInt(const QString& key)
{
    return !m_container.contains(key) ? 0 : m_container[key].toInt();
}

QString ConfigContainer::getString(const QString& key)
{
    return !m_container.contains(key) ? QString("") : m_container[key].toString();
}

void ConfigContainer::setDouble(const QString& key, double value)
{
    m_container[key] = QVariant(value);
}

void ConfigContainer::setInt(const QString& key, int value)
{
    m_container[key] = QVariant(value);
}

void ConfigContainer::setString(const QString& key, QString value)
{
    m_container[key] = QVariant(value);
}

bool ConfigContainer::getBool(const QString& key)
{
    return !m_container.contains(key) ? false : m_container[key].toBool();
}

void ConfigContainer::setBool(const QString& key, bool value)
{
    m_container[key] = QVariant(value);
}

Point3 ConfigContainer::getPoint3D(const QString& key)
{
    return !m_container.contains(key) ? Point3( 0, 0, 0) : m_container[key].value<Point3>();
}

void ConfigContainer::setPoint3D(const QString& key, const Point3 &v)
{
    m_container[key] = QVariant::fromValue(v);
}

