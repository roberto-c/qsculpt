#ifndef OBJECTPROPERTIES_H_
#define OBJECTPROPERTIES_H_

#include <QtGui/QWidget>
#include "ui_TransformWidget.h"

class ISurface;

class ObjectProperties: public QWidget, private Ui::TransformWidget
{
public:
	ObjectProperties();
	virtual ~ObjectProperties();
};

#endif /*OBJECTPROPERTIES_H_*/
