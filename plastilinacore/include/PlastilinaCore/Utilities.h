/*
 *  Utilities.h
 *  qsculpt
 *
 *  Created by Roberto Cabral on 2/27/11.
 *  Copyright 2011 plastilinaware. All rights reserved.
 *
 */

#ifndef UTILITIES_H_
#define UTILITIES_H_

namespace utils
{
template <typename Type>
Type* getReference(Type& p)
{
    return &p;
}
template <typename Type>
Type* getReference(Type* p)
{
    return p;
}

class Functor
{
  public:
    void operator()(){};
};
};

namespace core
{
namespace utils
{
template <typename TO, typename FROM>
void convert_to(const FROM& from, TO& to);

template <typename TO, typename FROM>
TO convert_to(const FROM& d)
{
    TO v;
    convert_to<TO>(d, v);
    return v;
}

std::string to_string(Eigen::Vector2f& v);
std::string to_string(Eigen::Vector3f& v);
std::string to_string(Eigen::Vector4f& v);

std::string get_app_path();

}; // namespace utils
}; // namespace core

#endif
