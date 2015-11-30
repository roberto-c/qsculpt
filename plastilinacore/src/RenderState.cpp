//
//  RenderState.cpp
//  QSculpt
//
//  Created by Juan Roberto Cabral Flores on 4/12/15.
//
//
#include <PlastilinaCore/Stable.h>
#include <PlastilinaCore/RenderState.h>
#include <stdio.h>

bool
RenderState::isValid() const
{
    if (currentNode == nullptr
        || this->camera == nullptr
        || this->root == nullptr
        || this->material == nullptr)
    {
        return false;
    }
    return true;
}