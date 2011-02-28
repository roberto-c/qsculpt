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


namespace utils {
    template<typename Type>
    Type* getReference(Type& p) {
        return &p;
    }
    template<typename Type>
    Type* getReference(Type* p) {
        return p;
    }
};

#endif
