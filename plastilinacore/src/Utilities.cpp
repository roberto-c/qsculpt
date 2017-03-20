/***************************************************************************
 *   Copyright (C) 2012 by Juan Roberto Cabral Flores                      *
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
#pragma once

#include <PlastilinaCore/Stable.h>
#include <PlastilinaCore/Utilities.h>

#include <boost/filesystem.hpp>
#include <cstdlib>
#include <stdexcept>
#include <strstream>

#ifdef __APPLE__
#include <mach-o/dyld.h> // for application directory
#endif

namespace core
{
namespace utils
{

std::string to_string(Eigen::Vector2f& v)
{
    std::stringstream strout;
    strout << "(" << v.x() << "," << v.y() << ")";
    return strout.str();
}
std::string to_string(Eigen::Vector3f& v)
{
    std::stringstream strout;
    strout << "(" << v.x() << "," << v.y() << "," << v.z() << ")";
    return strout.str();
}
std::string to_string(Eigen::Vector4f& v)
{
    std::stringstream strout;
    strout << "(" << v.x() << "," << v.y() << "," << v.z() << "," << v.w()
           << ")";
    return strout.str();
}

std::string get_app_path()
{
    std::vector<char> exepath;
    uint32_t          size = 0;
#if defined(__APPLE__)
    _NSGetExecutablePath(NULL, &size);
    if (size > 0)
    {
        exepath.resize(size + 1);
        if (_NSGetExecutablePath(exepath.data(), &size) != 0)
            exepath[0] = '\0';
    }
    else
    {
        exepath.push_back('\0');
    }
#elif defined(_WIN32)
    exepath.resize(MAX_PATH);
    size = GetModuleFileNameA(nullptr, exepath.data(), exepath.size());
    exepath.resize(size + 1);
#endif
    std::string             p(exepath.data());
    boost::filesystem::path path(p);

    if (boost::filesystem::is_regular_file(path))
    {
        return path.parent_path().string();
    }
    else if (boost::filesystem::is_directory(path))
    {
        return path.string();
    }

    return std::string();
}

}; // namespace utils
}; // namespace core