/*
 *  Stable.cpp
 *  QSculpt
 *
 *  Created by Roberto Cabral on 12/31/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */


#include "Stable.h"

uint qHash(const Eigen::Matrix<float, 3, 1, 2, 3, 1> &key)
{
	qint16 x = lrintf(key[0] * 1000.0f);
	qint16 y = lrintf(key[1] * 1000.0f);
	qint16 z = lrintf(key[2] * 1000.0f);
	quint64 d = (quint64)(x && 0xFFFF) << 32 | (quint64)(y & 0xFFFF) << 16 | (quint64)(z & 0xFFFF);
	return qHash(d);
}