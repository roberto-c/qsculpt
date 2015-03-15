//
//  Utils.h
//  qsculpt
//
//  Created by Juan Roberto Cabral Flores on 6/28/11.
//  Copyright 2011 plastilinaware. All rights reserved.
//
#ifndef MATH_UTILS_H
#define MATH_UTILS_H

namespace math {
    template<typename T>
    T clamp(T Value, T Min, T Max)
    {
        return (Value < Min)? Min : (Value > Max)? Max : Value;
    }
};


#endif
