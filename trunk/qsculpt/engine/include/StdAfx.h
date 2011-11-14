// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#ifndef STDAFX_H
#define STDAFX_H

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

// TODO: reference additional headers your program requires here

#include <exception>
#include <stdexcept>
#include <typeinfo>

#define EIGEN_INITIALIZE_MATRICES_BY_ZERO

#include <Eigen/Core>
#include <Eigen/StdVector>
#include <Eigen/Geometry>
#include <Eigen/LU>

namespace Eigen {
    typedef Affine3f Transform3f;    
};


unsigned int qHash(const Eigen::Matrix<float, 3, 1, 2, 3, 1> &key);
unsigned int qHash(const std::pair<int, int> & v);

#include <iterator>

#include <QtDebug>
#include <QtOpenGL>
#include <QString>
#include <QPointer>
#include <QScopedPointer>
#include <QSharedPointer>
#include <QSharedData>
#include <QExplicitlySharedDataPointer>
#include <QSharedDataPointer>

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

//#define NOT_IMPLEMENTED qWarning("%s %s", __PRETTY_FUNCTION__, "not implemented");
#define NOT_IMPLEMENTED throw std::runtime_error("Not implemented");

#endif

