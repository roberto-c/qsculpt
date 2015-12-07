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

DLLEXPORT void Logging_increaseIndentation();
DLLEXPORT void Logging_decreaseIndentation();

class DLLEXPORT Logging {
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


#if (defined(DEBUG) || defined(_DEBUG)) && !defined(PLASTILINA_TRACE_DISABLE)
#	ifdef _MSC_VER
#		define TRACEFUNCTION(msg) Logging tracelog(std::string(__FUNCTION__) + std::string(#msg))
#	else
#		define TRACEFUNCTION(msg) Logging tracelog(std::string(__PRETTY_FUNCTION__) + std::string(#msg))
#	endif
#else
#	define TRACEFUNCTION(msg)
#endif /* DEBUG */

#endif
