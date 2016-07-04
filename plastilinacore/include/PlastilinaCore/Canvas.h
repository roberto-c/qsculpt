/***************************************************************************
*   Copyright (C) 2016 by Juan Roberto Cabral Flores                      *
*   roberto.cabral@gmail.com                                              *
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
#pragma once

class Color;

namespace gl { class Texture2D; }
namespace core
{

class Canvas
{
public:
    enum CanvasBackEnd {
        Unknown,
        OpenGL,
        OpenCL,
        GlTexture,
        ClTexture
    };
    static std::unique_ptr<Canvas> factory(CanvasBackEnd backend, int w, int h);

    Canvas(int w = 1280, int h=720);
    virtual ~Canvas();

    virtual void setPenColor(const Color & c) = 0;
    virtual void setFillColor(const Color & c) = 0;

    virtual void begin() = 0;
    virtual void end() = 0;
    virtual void drawRectangle(float x, float y, float w, float h) = 0;
    virtual void drawEllipse(float x, float y, float w, float h) = 0;

    virtual void applyFilter() = 0;

    virtual std::shared_ptr<gl::Texture2D> colorTexture() = 0;
    virtual std::shared_ptr<gl::Texture2D> depthTexture() = 0;
};


};
