//
//  ResourcesManager.cpp
//  qsculpt
//
//  Created by Juan Cabral on 7/5/12.
//  Copyright (c) 2012 plastilinaware. All rights reserved.
//
#include <PlastilinaCore/Plastilina.h>
#include <iostream>

#ifdef __APPLE__
#include <CoreFoundation/CoreFoundation.h>
#endif

#include <PlastilinaCore/ResourcesManager.h>


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
#else
std::string ResourcesManager::findResourcePath(const std::string name,
											   const std::string type)
{
	NOT_IMPLEMENTED;
	
	std::string path;
	
	return path;
}

#endif /* __APPLE__ */


