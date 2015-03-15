//
//  Logging.h
//  qsculpt
//
//  Created by Juan Cabral on 6/6/12.
//  Copyright (c) 2012 plastilinaware. All rights reserved.
//

#ifndef qsculpt_Logging_h
#define qsculpt_Logging_h

#include <string>
#include <iostream>

void Logging_increaseIndentation();
void Logging_decreaseIndentation();

class Logging {
    std::string msg;
    
public:
    Logging(const std::string & msg) : msg(msg) {
        std::cerr << ">>" << msg << std::endl;
        Logging_increaseIndentation();
    }
    
    ~Logging() {
        Logging_decreaseIndentation();
        std::cerr << "<<" << msg << std::endl;
    }
};


#if defined(DEBUG) && !defined(PLASTILINA_TRACE_DISABLE)
#define TRACEFUNCTION(msg) Logging tracelog(std::string(__PRETTY_FUNCTION__) + std::string(#msg))
#else
#define TRACEFUNCTION()
#endif /* DEBUG */

#endif
