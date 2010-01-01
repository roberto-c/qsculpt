/***************************************************************************
 *   Copyright (C) 2006 by Juan Roberto Cabral Flores   *
 *   roberto.cabral@gmail.com   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef SUBDIVIDECOMMAND_H
#define SUBDIVIDECOMMAND_H

#include <QThread>
#include "CommandBase.h"

class IObject3D;
struct Face;

/**
 * Subdivision command. Subdvide the selected object.
 *
 * @author Juan Roberto Cabral Flores <roberto.cabral@gmail.com>
 */
class SubdivideCommand : public CommandBase
{
    Q_OBJECT

public:
    SubdivideCommand();

	SubdivideCommand(const SubdivideCommand& cpy);

    ~SubdivideCommand();

    // ICommand Interface
	virtual ICommand* clone() const;
	virtual bool needsUserInteraction() const { return false; };
    virtual void execute();
	virtual void undo();
	virtual void redo();
	virtual QWidget* getOptionsWidget(){return NULL;}
	// End ICommand Interface

private:
    class WorkerThread;

    WorkerThread* m_workerThread;
};

class SubdivideCommand::WorkerThread : public QThread
{
    Q_OBJECT

public:
    virtual void run();

    void setRangeBegin(unsigned int value);
    void setRangeEnd(unsigned int value);
    void setObject3D(IObject3D* obj);

private:
    void subdivide(IObject3D* obj, int rbegin, int rend);
	
	void subdivideFace(IObject3D* obj, int faceIndex);

    void adjustPointNormal(IObject3D* obj, int index);

    Point3 computeFaceNormal(const IObject3D* obj, int index);

    Point3 computeFaceNormal(const IObject3D* obj, const Face &face);
signals:
    void progress(int value);

private:
	unsigned int m_rbegin;
    unsigned int m_rend;
    IObject3D* m_obj;
};


#endif
