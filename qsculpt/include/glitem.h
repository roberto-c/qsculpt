/***************************************************************************
 *   Copyright (C) $YEAR$ by Juan Roberto Cabral Flores   *
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

#ifndef GLITEM_H
#define GLITEM_H

class GlCanvas;
class ISurface;

/**
 * Interface class to define a renderer for the item.
 */
// class IGlItemRenderer
//{
// public:
//};

namespace Plastilina
{
enum ItemFlag
{
    None     = 0,
    Selected = 0x00000001
};

/**
 * Render item in the GlCanvas
 */
class GlItem
{
    ISurface* _s;

  public:
    GlItem();

    virtual ~GlItem();

    virtual void paintGl(GlCanvas* c);

    /**
     * Set a pointer to a user data.
     */
    void setData(void* d);

    /**
     * Returns the data attached to this item.
     */
    void* data();

    /**
     *
     */
    void setSelected(bool value);

    /**
     *
     */
    bool isSelected();
};
};
#endif // GLITEM_H
