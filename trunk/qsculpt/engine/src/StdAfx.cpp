// stdafx.cpp : source file that includes just the standard includes
// libsolidpaint3d.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "StdAfx.h"

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file
uint qHash(const Eigen::Matrix<float, 3, 1, 2, 3, 1> &key)
{
	qint16 x = lrintf(key[0] * 1000.0f);
	qint16 y = lrintf(key[1] * 1000.0f);
	qint16 z = lrintf(key[2] * 1000.0f);
	quint64 d = (quint64)(x && 0xFFFF) << 32 | (quint64)(y & 0xFFFF) << 16 | (quint64)(z & 0xFFFF);
	return qHash(d);
}

uint qHash(const std::pair<int, int> & v)
{
    return qHash(v.first ^ (v.second << 8));
}
