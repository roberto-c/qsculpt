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
#ifndef SCENE_H
#define SCENE_H

#include <QObject>
#include <string>
#include <QStandardItemModel>
#include "IIterator.h"

class ISurface;
class SceneNode;

/**
 * Contains all the scene data. All 3D objects reference data from the scene.
 *
 * @author Juan Roberto Cabral Flores <roberto.cabral@gmail.com>
*/
class Scene : public QStandardItemModel {
public:
    Scene();
    Scene(const std::string& name);

    ~Scene();

    /**
     *
     */
    SceneNode* findByName(const QString& name);

    /**
     * Returns the node with the specified instance ID. NULL if not found.
     */
    SceneNode* findByIID(uint iid);
    
    /**
     *
     */
    Iterator<SceneNode> iterator();
    
    /**
     *
     */
    Iterator<SceneNode> constIterator() const;
};

#endif

