/*
 *  brushproperties.cpp
 *  solidpaint3d
 *
 *  Created by Roberto Cabral on 1/2/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "brushproperties.h"

//using namespace Command::Ui;

BrushProperties::BrushProperties(QWidget* parent)
: QWidget(parent)
{
	setupUi(this);
}

BrushProperties::~BrushProperties()
{
}

double BrushProperties::getBrushRadius()
{
	return spinRadius->value();
}

void BrushProperties::setBrushRadius(double radius)
{
	if (radius > 0.00001)
		spinRadius->setValue(radius);
}

double BrushProperties::getBrushStrength()
{
	return spinStrength->value();
}

void BrushProperties::setBrushStrength(double strength)
{
	spinStrength->setValue(strength);
}

BrushCommand::BrushAction BrushProperties::getBrushAction()
{
	if (radPush->isChecked())
		return BrushCommand::Push;
	else if (radPull->isChecked())
		return BrushCommand::Pull;
	
	return BrushCommand::Push;
}

void BrushProperties::setBrushAction(BrushCommand::BrushAction action)
{
	switch(action)
	{
		case BrushCommand::Pull:
			radPull->setChecked(true);
			break;
		case BrushCommand::Push:
			radPush->setChecked(true);
			break;
	}
}
