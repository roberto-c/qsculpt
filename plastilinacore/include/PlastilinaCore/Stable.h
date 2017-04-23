// Stable.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//
#pragma once
#ifndef STABLE_H
#define STABLE_H

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers
#define NOMINMAX
#include <windows.h>
#endif

// TODO: reference additional headers your program requires here

#if defined __cplusplus
#include <PlastilinaCore/Plastilina.h>
#endif

#endif
