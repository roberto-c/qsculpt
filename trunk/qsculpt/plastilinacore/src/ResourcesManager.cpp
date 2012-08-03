//
//  ResourcesManager.cpp
//  qsculpt
//
//  Created by Juan Cabral on 7/5/12.
//  Copyright (c) 2012 plastilinaware. All rights reserved.
//
#include "Plastilina.h"
#include <iostream>

#include "ResourcesManager.h"


struct ResourcesManagerData {
    std::string path;
};

ResourcesManagerData g_data;

void ResourcesManager::setResourcesDirectory(const std::string &path)
{
    g_data.path = path;
}

std::string ResourcesManager::resourcesDirectory()
{
    return g_data.path;
}