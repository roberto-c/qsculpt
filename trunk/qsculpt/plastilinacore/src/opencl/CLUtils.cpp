//
//  CLUtils.cpp
//  PlastilinaCore
//
//  Created by Juan Roberto Cabral Flores on 12/9/12.
//
//
#include <PlastilinaCore/Stable.h>

#include <fstream>
#include <string>

#include <PlastilinaCore/opencl/CLUtils.h>

std::string opencl::loadFromFile(const std::string &filename)
{
	std::string	source;
    std::ifstream file(filename);
	
    std::string line;
    while (file.good()) {
        getline(file, line);
        line.append("\n");
        source.append(line);
    }
    return source;
}