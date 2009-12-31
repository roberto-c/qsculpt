/*
 *  SceneNode.h
 *  QSculpt
 *
 *  Created by Roberto Cabral on 12/18/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
#include <vector>

class SceneNode
{
	SceneNode* _parent;
	std::vector<SceneNode*> _children;
	Eigen::Matrix4f _transformation;
	
};
