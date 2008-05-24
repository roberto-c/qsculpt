// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#ifndef STDAFX_H
#define STDAFX_H

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

// TODO: reference additional headers your program requires here
#include <QtDebug>
#include <QtOpenGL>

inline bool printGlError()
{
	bool result = false;
	GLenum error = glGetError();
	while( error != GL_NO_ERROR )
	{
		result = true;
		const GLubyte* strError = gluErrorString(error);
		qDebug()<<"GLError: code: " << error << " " << (const char*)strError;
		error = glGetError();
	}
	return result;
}

#endif

