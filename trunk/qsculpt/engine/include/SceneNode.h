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

#include <vector>
#include <QObject>

/**
 * Basic scene graph node class. 
 */
class SceneNode
{
    //Q_OBJECT
    
	SceneNode* _parent;
	Eigen::Transform3f _transform;
	
public:
    SceneNode(SceneNode* parent){};
    virtual ~SceneNode(){};
    
    const Eigen::Transform3f& transform() const { return _transform; };
    Eigen::Transform3f& transform() { return _transform; };
    void setTransform(const Eigen::Transform3f& t) {};
    
    
};

class GroupNode : public SceneNode
{
    std::vector<SceneNode*> _group;
    
public:
    GroupNode(SceneNode *parent);
    ~GroupNode();
    
    void add(SceneNode *n);
    void remove(SceneNode *n);
};
