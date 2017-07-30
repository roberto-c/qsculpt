/***************************************************************************
 *   Copyright (C) 2017 by Juan Roberto Cabral Flores                      *
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
#include <PlastilinaCore/Stable.h>

#include <PlastilinaCore/opengl/Context.h>

#define THROW_IF(condition,msg)  if ((condition)) { throw std::runtime_error(msg); }

namespace gl
{

struct Context::Impl
{
    bool              initialized;
    gl::Device        device;
    intptr_t          glrc;
    intptr_t          nativeWindow;
    intptr_t          osDevice;
    intptr_t          hInstance;

    Impl()
        : initialized(false)
        , glrc(0)
        , nativeWindow(0)
        , osDevice(0)
        , hInstance(0)
    {
    }

    bool initializeGL();
};

bool Context::Impl::initializeGL()
{
    return true;
}

Context::Context(const core::GraphicsContextCreateInfo & createInfo)
    : d(new Impl())
{
    if (!d->initialized)
        d->initialized = d->initializeGL();

    if (createInfo.osContext != 0)
    {
        //d->glrc = reinterpret_cast<HGLRC>(createInfo.osContext);
        //d->osDevice = wglGetCurrentDC();
        return;
    }

    std::vector<int> attList;
    std::vector<int> pixelFormatAttList;
    //d->nativeWindow = reinterpret_cast<HWND>(createInfo.osWindowHandle);
    //d->osDevice = reinterpret_cast<HDC>(createInfo.osHandleEx);

    for (auto att : createInfo.pixelFormatAttributes)
    {
        pixelFormatAttList.push_back(att);
    }

//    int pixelFormat;
//    UINT numFormats;
//    BOOL res = wglChoosePixelFormatARB(d->osDevice, pixelFormatAttList.data(),
//        nullptr, 1, &pixelFormat, &numFormats);
//    THROW_IF((res == FALSE), "Failed to find a suitable pixel format");
//    for (auto att : createInfo.attributesList)
//    {
//        attList.push_back(att);
//    }
//    THROW_IF((wglCreateContextAttribsARB == nullptr), "wglCreateContextAttribsARB not available");
//    d->glrc = wglCreateContextAttribsARB(d->osDevice, nullptr, attList.data());
//    if (d->glrc == nullptr)
//    {
//        DWORD errorCode = GetLastError();
//        TRACE(error) << "Errorcode:" << errorCode << " hex: " << std::hex << errorCode << std::dec;
//    }
//    THROW_IF((d->glrc == nullptr), "Failed to create an OpenGL context");
}

Context::~Context()
{
}

intptr_t Context::windowHandle() const
{
    return reinterpret_cast<intptr_t>(d->nativeWindow);
}

intptr_t Context::nativeDeviceHandle() const
{
    return reinterpret_cast<intptr_t>(d->osDevice);
}

intptr_t Context::nativeGlContext() const
{
    return reinterpret_cast<intptr_t>(d->glrc);
}

bool Context::makeCurrent()
{
    return false; // wglMakeCurrent(d->osDevice, d->glrc) == TRUE;
}

bool Context::swapBuffers()
{
    return false; // SwapBuffers(d->osDevice) == TRUE;
}

core::IDevice* Context::device()
{
    return nullptr;
}

void Context::resize(int w, int h)
{

}

};
