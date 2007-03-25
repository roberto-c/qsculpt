/*
 *  brushproperties.h
 *  solidpaint3d
 *
 *  Created by Roberto Cabral on 1/2/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef BRUSHPROPERTIES_H
#define BRUSHPROPERTIES_H

#include <QWidget>
#include "../ui/ui_BrushProperties.h"
#include "brushcommand.h"

//namespace Command { namespace Ui {
	
class BrushProperties : public QWidget, private Ui::BrushProperties
{
	Q_OBJECT
	
public:
	BrushProperties(QWidget* parent);
	virtual ~BrushProperties();
	
// public signals:
	double getBrushRadius();
	void setBrushRadius(double radius);
	
	double getBrushStrength();
	void setBrushStrength(double strength);
	
	BrushCommand::BrushAction getBrushAction();
	void setBrushAction(BrushCommand::BrushAction action);
};

//};};

#endif
