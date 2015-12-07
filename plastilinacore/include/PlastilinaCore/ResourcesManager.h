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
     * 
     */
    static void setResourcesDirectory(const std::string & path);
    
    static std::string resourcesDirectory();
	
	std::string findResourcePath(const std::string name,
								 const std::string type,
								 const std::string subdir = "");
};
    
    



#endif
