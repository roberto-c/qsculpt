//
//  ResourcesManager.cpp
//  qsculpt
//
//  Created by Juan Cabral on 7/5/12.
//  Copyright (c) 2012 plastilinaware. All rights reserved.
//
#include <PlastilinaCore/Stable.h>
#include <PlastilinaCore/Plastilina.h>
#include <iostream>
#include <boost/filesystem.hpp>
#include <mutex>

#ifdef __APPLE__
#include <CoreFoundation/CoreFoundation.h>
#endif

#include <PlastilinaCore/ResourcesManager.h>


struct ResourcesManagerData {
    std::vector<std::string> path;
};

std::mutex g_resourcesManagerLock;
ResourcesManagerData g_data;

void ResourcesManager::setResourcesDirectory(const std::string &path)
{
    std::lock_guard<std::mutex> lock(g_resourcesManagerLock);
    g_data.path.clear();
    g_data.path.push_back(path);
}

void ResourcesManager::addResourcesDirectory(const std::string &path)
{
    std::lock_guard<std::mutex> lock(g_resourcesManagerLock);
    g_data.path.insert(g_data.path.cbegin(), path);
}

std::string ResourcesManager::resourcesDirectory()
{
    std::lock_guard<std::mutex> lock(g_resourcesManagerLock);
    return g_data.path[0];
}

std::vector<std::string> ResourcesManager::resourcesDirectories()
{
    std::lock_guard<std::mutex> lock(g_resourcesManagerLock);
    return g_data.path;
}


#ifdef __APPLE__
std::string ResourcesManager::findResourcePath(const std::string name,
											   const std::string type,
											   const std::string subdir)
{
	std::string path;
	
	CFURLRef    resourceURL = NULL;
	CFStringRef resourceName = CFStringCreateWithCString( NULL,
														 name.c_str(),
														 kCFStringEncodingUTF8);
	CFStringRef cfstrType = CFStringCreateWithCString(NULL,
													  type.c_str(),
													  kCFStringEncodingUTF8);
	CFStringRef cfSubfir = CFStringCreateWithCString(NULL,
													 subdir.c_str(),
													 kCFStringEncodingUTF8);

	CFBundleRef requestedBundle;
	requestedBundle = CFBundleGetMainBundle();
	if (requestedBundle) {
		// Look for a resource in the main bundle by name and type.
		resourceURL = CFBundleCopyResourceURL( requestedBundle,
											  resourceName,
											  cfstrType,
											  cfSubfir );
	}
	if (!resourceURL) {
		std::cerr << "Failed to find resourceURL. Trying framework bundle" << std::endl;
		// Look for a bundle using its identifier
		requestedBundle = CFBundleGetBundleWithIdentifier(CFSTR("com.plastilinalabs.PlastilinaCore") );
		if (requestedBundle) {
			// Look for a resource in the main bundle by name and type.
			resourceURL = CFBundleCopyResourceURL( requestedBundle,
												  resourceName,
												  cfstrType,
												  cfSubfir );
		}
	}
	if (resourceURL) {
		UInt8 aPath[1024] = {0};
		if (!CFURLGetFileSystemRepresentation(resourceURL,
											  FALSE,
											  aPath,
											  sizeof(aPath)))
		{
			std::cerr << "Failed to allocate string" << std::endl;
		} else {
			path = std::string((char*)aPath);
		}
	}
	if (resourceURL) CFRelease(resourceURL);
	if (resourceName) CFRelease(resourceName);
	if (cfstrType) CFRelease(cfstrType);
	if (cfSubfir) CFRelease(cfSubfir);

	
	return path;
}
#elif defined(_WIN32)
std::string ResourcesManager::findResourcePath(const std::string name,
    const std::string type,
    const std::string subdir)
{
    using std::string;
    using namespace boost::filesystem;
    
    path fname(name + string(".") + type);
    for (auto pathstr : this->resourcesDirectories())
    {
        path p(pathstr);
        if (is_directory(p))
        {
            TRACE(trace) << "Searching under " << p.string();
            for (directory_entry& x : directory_iterator(p))
            {
                if (x.path().filename().compare(fname) == 0)
                {
                    TRACE(trace) << "Found: " << x.path().string();
                    return x.path().string();
                }
            }
        }
        p = p.parent_path().append("share");
        if (is_directory(p))
        {
            TRACE(trace) << "Searching under " << p.string();
            for (directory_entry& x : directory_iterator(p))
            {
                if (x.path().filename().compare(fname) == 0)
                {
                    TRACE(trace) << "Found: " << x.path().string();
                    return x.path().string();
                }
            }
        }
    }
    throw std::runtime_error("resource name: " + name + " not found");
}
#else
std::string ResourcesManager::findResourcePath(const std::string name,
    const std::string type,
    const std::string subdir)
{
	NOT_IMPLEMENTED;
	
	std::string path;
	
	return path;
}

#endif /* __APPLE__ */


