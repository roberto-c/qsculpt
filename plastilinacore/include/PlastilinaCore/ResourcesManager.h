//
//  ResourcesManager.h
//  qsculpt
//
//  Created by Juan Cabral on 7/5/12.
//  Copyright (c) 2012 plastilinaware. All rights reserved.
//

#ifndef qsculpt_ResourcesManager_h
#define qsculpt_ResourcesManager_h

#include <string>

class DLLEXPORT ResourcesManager {
public:
    /**
     * Set a directory where all assets and resources are located.
     *
     * This will replace all directory set before.
     */
    static void setResourcesDirectory(const std::string & path);

    /**
    * Add a directory to the search path where assets and resources are located.
    */
    static void addResourcesDirectory(const std::string & path);
    
    static std::string resourcesDirectory();

    static std::vector<std::string> resourcesDirectories();
	
	std::string findResourcePath(const std::string name,
								 const std::string type,
								 const std::string subdir = "");
};
    
    



#endif
