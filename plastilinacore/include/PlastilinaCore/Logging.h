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
#include <boost/log/trivial.hpp>

#define TRACE BOOST_LOG_TRIVIAL

#if (defined(DEBUG) || defined(_DEBUG)) && !defined(PLASTILINA_TRACE_DISABLE)
#	ifdef _MSC_VER
#		define TRACEFUNCTION(msg) TRACE(trace) << std::string(__FUNCTION__) << std::string(#msg)
#	else
#       define TRACEFUNCTION(msg) TRACE(trace) << std::string(__PRETTY_FUNCTION__) << std::string(#msg)
#	endif
#else
#	define TRACEFUNCTION(msg)
#endif /* DEBUG */

#endif
