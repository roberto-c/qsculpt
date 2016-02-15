// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//
#pragma once
#ifndef STABLE_H
#define STABLE_H

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#define NOMINMAX
#include <windows.h>
#endif

// TODO: reference additional headers your program requires here
// We are not ready to use OpenCL 2.0. So use deprecated features
#define CL_USE_DEPRECATED_OPENCL_2_0_APIS

#if defined __cplusplus

#include <PlastilinaCore/Plastilina.h>
#include <PlastilinaCore/Logging.h>
#endif

#endif

